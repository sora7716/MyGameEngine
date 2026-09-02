#include "DebugCamera.h"
#include "MathUtility.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "Quaternion.h" 
#include "GameObject.h"
#include "MatrixUtility.h"
#include <algorithm>

//コンストラクタ
DebugCamera::DebugCamera(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
DebugCamera::~DebugCamera(){
}

//初期化
void DebugCamera::Initialize(){
	//基底クラスの初期化
	Component::Initialize();
	gameObject_ = GetOwner();
	camera_ = gameObject_->GetComponent<Camera>();

	assert(camera_);

	fovY_ = camera_->GetFovY();
}

//更新
void DebugCamera::Update(){
	//基底クラスの更新
	Component::Update();
#ifdef USE_IMGUI
	//エスケープキーを入力したら
	if (ImGui::IsKeyPressed(ImGuiKey_Escape)){
		isControlEnabled_ = !isControlEnabled_;
	}
#endif // USE_IMGUI

	//デバッグモードがfalseだった場合
	if (!isControlEnabled_){
		return;
	}

	//平行移動の更新
	TranslateUpdate();

	//回転の操作
	RotateControl();

	//ズーム操作
	ZoomControl();
}

//複製
std::unique_ptr<Component> DebugCamera::Clone(GameObject* gameObject) const{
	std::unique_ptr<DebugCamera>cloneInstance = std::make_unique<DebugCamera>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//DebugCameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->fovY_ = this->fovY_;
	return cloneInstance;
}

//左右移動の操作
void DebugCamera::StrafeControl(){
#ifdef USE_IMGUI
	//横移動
	if (ImGui::IsKeyDown(ImGuiKey_A)){
		moveDir_.x = -1.0f;
	} else if (ImGui::IsKeyDown(ImGuiKey_D)){
		moveDir_.x = 1.0f;
	} else{
		moveDir_.x = 0.0f;
	}
#endif // USE_IMGUI
}

//上下移動の操作
void DebugCamera::ElevateControl(){
#ifdef USE_IMGUI
	if (ImGui::IsKeyDown(ImGuiKey_Q)){
		moveDir_.y = -1.0f;
	} else if (ImGui::IsKeyDown(ImGuiKey_E)){
		moveDir_.y = 1.0f;
	} else{
		moveDir_.y = 0.0f;
	}
#endif // USE_IMGUI
}

//前後移動の操作
void DebugCamera::DollyControl(){
#ifdef USE_IMGUI
	if (ImGui::IsKeyDown(ImGuiKey_W)){
		moveDir_.z = 1.0f;
	} else if (ImGui::IsKeyDown(ImGuiKey_S)){
		moveDir_.z = -1.0f;
	} else{
		moveDir_.z = 0.0f;
	}
#endif // USE_IMGUI
}

//ズーム操作
void DebugCamera::ZoomControl(){
#ifdef USE_IMGUI
	//ImGuiを使用していた場合
	if (ImGui::GetIO().WantCaptureMouse){
		return;
	}

	//マウスホイールの回転量でズームイン、ズームアウト
	fovY_ -= ImGui::GetIO().MouseWheel * kZoomSpeedMagnification;
	//ズーム操作のリセット
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)){
		fovY_ = 0.45f;
	}
	//fovYを範囲内で止める
	fovY_ = std::clamp(fovY_, kMinFovY, kMaxFovY);

	if (camera_){
		//fovYのセット
		camera_->SetFovY(fovY_);
	}
#endif // USE_IMGUI
}

//回転の操作
void DebugCamera::RotateControl(){
	Vector3 rotate = {};
#ifdef USE_IMGUI
	//ImGuiを使用していた場合
	if (ImGui::GetIO().WantCaptureMouse){
		return;
	}

	//マウスのフリックを取得
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)){
		ImVec2 mouseFlick = ImGui::GetIO().MouseDelta;
		//フリックの値をVector2に格納
		mouseFlick_.x = mouseFlick.x;
		mouseFlick_.y = mouseFlick.y;

		//フリックの値をカメラの回転に反映
		rotate.x += mouseFlick_.y * kLookRadPerCount;
		rotate.y += mouseFlick_.x * kLookRadPerCount;
		gameObject_->GetTransform().SetEulerAngle(rotate);
	}
#endif // USE_IMGUI
}

//平行移動の更新
void DebugCamera::TranslateUpdate(){
#ifdef USE_IMGUI
	//ImGuiを使用していた場合
	if (ImGui::GetIO().WantCaptureKeyboard){
		return;
	}
#endif // USE_IMGUI
	//X軸方向の移動
	StrafeControl();

	//Y軸方向の移動
	ElevateControl();

	//Z軸方向の移動
	DollyControl();

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = matrixUtility::MakeRotateMatrix(gameObject_->GetTransform().quaternion);

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = mathUtility::TransformNormal(Vector3(moveDir_.x, 0.0f, moveDir_.z), rotMat);

	//Y軸のそのまま
	moveDir_ = { moveDirXZ.x,moveDir_.y,moveDirXZ.z };

	//カメラを移動させる
	gameObject_->GetTransform().translate += moveDir_.Normalize() * kMoveSpeed;
}