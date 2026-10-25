#include "GizmoSystem.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "GameObject.h"
#include "Object3d.h"
#include "TextureManager.h"
#include <cassert>

//生成
std::unique_ptr<GizmoSystem> GizmoSystem::Create(){
	std::unique_ptr<GizmoSystem>instance = std::make_unique<GizmoSystem>();
	return instance;
}

//コンストラクタ
GizmoSystem::GizmoSystem(){
}

//デストラクタ
GizmoSystem::~GizmoSystem(){
}

//ツールバーの描画
void GizmoSystem::DrawToolbar(TextureManager* textureManager){
#ifdef USE_IMGUI
	//ツールバーが非表示の場合
	if (!isToolbarVisible_){
		return;
	}

	//画像サイズ
	float imageSize = 32.0f;
	//現在のスタイル
	const ImGuiStyle& style = ImGui::GetStyle();
	//ボタンのサイズ
	const ImVec2 buttonSize = { imageSize,imageSize };
	//ウィンドウサイズ
	const ImVec2 windowSize = {
		imageSize + style.FramePadding.x * 2.0f + style.WindowPadding.x * 2.0f,
		(imageSize + style.FramePadding.y * 2.0f) * 3.0f + style.ItemSpacing.y * 2.0f + style.WindowPadding.x * 2.0f
	};

	//子ウィンドウを作成
	ImGui::BeginChild("ツールバー", windowSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	//各アイコンのハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE scaleIconHandle = textureManager->GetSRVHandleGPU(directoryPath_ + "scale.png");
	D3D12_GPU_DESCRIPTOR_HANDLE rotateIconHandle = textureManager->GetSRVHandleGPU(directoryPath_ + "rotate.png");
	D3D12_GPU_DESCRIPTOR_HANDLE translateIconHandle = textureManager->GetSRVHandleGPU(directoryPath_ + "translate.png");

	//拡縮
	if (ImGui::ImageButton("Scale", reinterpret_cast<ImTextureID>(scaleIconHandle.ptr), buttonSize)){
		gizmoTool_ = GizmoTool::kScale;
	}

	//回転
	if (ImGui::ImageButton("Rotate", reinterpret_cast<ImTextureID>(rotateIconHandle.ptr), buttonSize)){
		gizmoTool_ = GizmoTool::kRotate;
	}

	//平行移動
	if (ImGui::ImageButton("Translate", reinterpret_cast<ImTextureID>(translateIconHandle.ptr), buttonSize)){
		gizmoTool_ = GizmoTool::kTranslate;
	}
	ImGui::EndChild();
#endif // USE_IMGUI
}

//選択中のオブジェクトの Transform を操作する
bool GizmoSystem::ManipulateSelectedTransform(const Vector2& sceneViewPosition, const Vector2& sceneViewSize){
#ifdef USE_IMGUI
	//操作対象が存在するか
	if (!selectedGameObject_){
		return false;
	}

	//デバッグカメラがあるか
	if (!debugCamera_){
		return false;
	}

	ImGuizmo::SetDrawlist();

	//どれくらいの範囲で描画するか
	ImGuizmo::SetRect(
		sceneViewPosition.x,
		sceneViewPosition.y,
		sceneViewSize.x,
		sceneViewSize.y
	);

	//ビュー行列を取得
	Matrix4x4 viewMatrix = debugCamera_->GetViewMatrix();
	//透視投影行列を取得
	Matrix4x4 projectionMatrix = debugCamera_->GetProjectionMatrix();

	//選択しているGameObjectのワールド行列を取得
	Matrix4x4 worldMatrix = Matrix4x4::Identity4x4();

	//ノードのモデル行列
	Matrix4x4 modelMatrix = Matrix4x4::Identity4x4();
	//オブジェクトのワールド行列
	Matrix4x4 objectWorldMatrix = Matrix4x4::Identity4x4();

	//オブジェクトのワールド行列を設定
	objectWorldMatrix = selectedGameObject_->GetWorldMatrix();
	//ワールド行列を設定
	worldMatrix = objectWorldMatrix;

	//Nodeを選択した場合
	if (!selectedNodePath_.empty()){
		Object3d* selectedObject3d = selectedGameObject_->GetComponent<Object3d>();
		if (selectedObject3d){
			selectedObject3d->TryGetNodeModelMatrix(selectedNodePath_, modelMatrix);
			worldMatrix = modelMatrix * objectWorldMatrix;
		}
	}

	//実際に動かす
	bool isChangedMatrix = ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], static_cast<ImGuizmo::OPERATION>(gizmoTool_), ImGuizmo::WORLD, &worldMatrix.m[0][0]);

	//行列が変更されたか
	if (isChangedMatrix){
		//ローカル行列
		Matrix4x4 localMatrix = worldMatrix * selectedGameObject_->GetParentMatrix().Inverse();

		//ワールド行列からトランスフォームに分解
		Transform transform = matrixUtility::DecomposeMatrix(localMatrix, selectedGameObject_->GetTransform().scale);

		if (!selectedNodePath_.empty()){
			//ノードのローカル行列
			Matrix4x4 nodeLocalMatrix = Matrix4x4::Identity4x4();
			//ノードのベース行列
			Matrix4x4 nodeBaseMatrix = Matrix4x4::Identity4x4();
			Object3d* object3d = selectedGameObject_->GetComponent<Object3d>();
			if (object3d){
				Node node = {};
				//ノードを取得
				object3d->TryGetNodeForPath(selectedNodePath_, node);
				nodeLocalMatrix = node.localMatrix;
				nodeBaseMatrix = node.baseMatrix;
			}
			//操作後のモデル行列
			Matrix4x4 newModelMatrix = worldMatrix * objectWorldMatrix.Inverse();
			//親ノードのモデル行列
			Matrix4x4 parentNodeMatrix = nodeLocalMatrix.Inverse() * modelMatrix;
			//操作後のローカル行列
			Matrix4x4 newModelLocal = newModelMatrix * parentNodeMatrix.Inverse();
			//ローカル行列
			Matrix4x4 newNodeLocalMatrix = newModelLocal * nodeBaseMatrix.Inverse();
			//Transformを求める
			Transform localNodeTransform = matrixUtility::DecomposeMatrix(newNodeLocalMatrix, selectedGameObject_->GetTransform().scale);
			//NodeのLocalTransformに設定
			object3d->SetNodeLocalTransform(selectedNodePath_, localNodeTransform);
		} else{
			//Transformを設定
			selectedGameObject_->GetTransform() = transform;
		}
	}
	return true;
#endif // USE_IMGUI
}

//デバッグカメラの設定
void GizmoSystem::SetDebugCamera(Camera* debugCamera){
	debugCamera_ = debugCamera;
}

//選択されたゲームオブジェクトを設定
void GizmoSystem::SetSelectedGameObject(GameObject* selectedGameObject){
	selectedGameObject_ = selectedGameObject;
}

//選択したノードパスを設定
void GizmoSystem::SetSelectedNodePath(const std::string& selectedNodePath){
	selectedNodePath_ = selectedNodePath;
}

//Gizmoツールバーを表示するかの取得
bool& GizmoSystem::IsToolbarVisible(){
	return isToolbarVisible_;
}
