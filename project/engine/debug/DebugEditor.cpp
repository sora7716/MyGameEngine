#define NOMINMAX
#include "DebugEditor.h"
#include "GameObject.h"
#include "TagManager.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "MatrixUtility.h"
#include "TextureManager.h"
#include "DebugCameraController.h"
#include "Object3d.h"
#include "Component.h"
#include "PropertyRegistry.h"
#include <cassert>
#include <array>

//コンストラクタ
DebugEditor::DebugEditor(){
}

//デストラクタ
DebugEditor::~DebugEditor(){
}

//初期化
void DebugEditor::Initialize(TextureManager* textureManager, TagManager* tagManager){
	//タグの管理を記録
	assert(tagManager);
	tagManager_ = tagManager;
	//テクスチャの管理を記録
	assert(textureManager);
	textureManager_ = textureManager;

	//選択するオブジェクトの初期化
	selectedGameObject_ = nullptr;
	gameObjects_ = nullptr;

	//作成要求
	requestCreateGameObject_ = false;
	//複製要求
	requestDuplicateGameObject_ = nullptr;
	//削除要求
	requestDeleteGameObject_ = nullptr;
	//名前変更
	renamingGameObject_ = nullptr;
}

//更新
void DebugEditor::Update(){
}

//描画
void DebugEditor::Draw([[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE sceneHandle, [[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE previewHandle){
#ifdef USE_IMGUI
	//ドッキングスペースの描画
	DrawDockSpace();
	//ヒエラルキーの描画
	DrawHierarchy();
	//プレビューの描画
	DrawPreview(previewHandle);
	//シーンの描画
	DrawScene(sceneHandle);
	//インスペクターの描画
	DrawInspector();
	//タグの管理の描画
	if (isTegManagerOpen_){
		DrawTagManager();
	}
#endif // USE_IMGUI
}

//ゲームオブジェクト一覧の設定
void DebugEditor::SetGameObjects(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	gameObjects_ = &gameObjects;
}

//デバッグカメラの設定
void DebugEditor::SetDebugCamera(Camera* debugCamera){
	debugCamera_ = debugCamera;
}

//削除要求を取得
bool DebugEditor::ConsumeDeleteRequest(GameObject*& target){
	//削除のリクエストがNullじゃなければ
	if (!requestDeleteGameObject_){
		return false;
	}

	target = requestDeleteGameObject_;
	requestDeleteGameObject_ = nullptr;

	//選択中のオブジェクトを削除する場合
	if (selectedGameObject_ == target){
		selectedGameObject_ = nullptr;
	}

	return true;
}

//複製要求を取得
bool DebugEditor::ConsumeDuplicateRequest(GameObject*& target){
	//複製の要求がNullじゃなければ
	if (!requestDuplicateGameObject_){
		return false;
	}

	target = requestDuplicateGameObject_;
	requestDuplicateGameObject_ = nullptr;

	return true;
}

//生成要求を取得
bool DebugEditor::ConsumeCreateRequest(){
	bool request = requestCreateGameObject_;
	requestCreateGameObject_ = false;
	return request;
}

//ゲームオブジェクトの移動要求を取得
bool DebugEditor::ConsumeMoveGameObjectRequest(uint32_t& from, uint32_t& to){
	if (!requestMoveGameObject_){
		return false;
	}

	//fromとtoを書き換え
	from = draggedIndex_;
	to = dropTargetIndex_;

	//リクエストをfalseに変更
	requestMoveGameObject_ = false;

	return true;
}

//タグ名変更の要求の取得
bool DebugEditor::ConsumeRenameTagRequest(std::string& oldTag, std::string& newTag){
	//要求されたタグ名があるか確認
	if (!requestRenameTag_){
		return false;
	}

	//あった場合
	oldTag = requestRenameTag_->oldTag;
	newTag = requestRenameTag_->newTag;

	requestRenameTag_.reset();
	return true;
}

//タグの削除の要求の取得
bool DebugEditor::ConsumeDeleteTagRequest(std::string& tag){
	//要求されたタグが存在するか
	if (!requestDeleteTag_){
		return false;
	}

	//存在した場合
	tag = *requestDeleteTag_;

	requestDeleteTag_.reset();

	return true;
}

//親子付けの要求の取得
bool DebugEditor::ConsumeParentRequest(ParentRequest& parentRequest){
	//親子付けのリクエストがNullじゃなければ
	if (!requestAttachTo_){
		return false;
	}

	//リクエストに代入
	parentRequest = *requestAttachTo_;
	//リクエストのリセット
	requestAttachTo_.reset();
	return true;
}

//親子付け解除の要求を取得
bool DebugEditor::ConsumeDetachRequest(GameObject*& target){
	//親子付けのリクエストがNullじゃなければ
	if (!requestDetach_){
		return false;
	}

	//リクエストに代入
	target = *requestDetach_;
	//リクエストのリセット
	requestDetach_.reset();
	return true;
}

//GameObjectを選択
void DebugEditor::SelectGameObject(GameObject* gameObject){
	//選択したノードのパスをクリア
	selectedNodePath_.clear();

	//選択したゲームオブジェクトが一致していたら
	if (selectedGameObject_ == gameObject){
		return;
	}

	//選択したオブジェクトに保存
	selectedGameObject_ = gameObject;

	//選択したオブジェクトがNullじゃなければ
	if (selectedGameObject_){
		inspectorEulerAngle_ = selectedGameObject_->GetTransform().GetEulerAngle();
	}
}

//シーンのImGuiウィンドウを選択しているかの取得
bool DebugEditor::IsSceneViewHovered() const{
	return isSceneViewHovered_;
}

//プレビューのImGuiウィンドウを選択しているかの取得
bool DebugEditor::IsPreviewHovered() const{
#ifdef _DEBUG
	return isPreviewHovered_;
#else
	return true;
#endif // _DEBUG
}

//ドッキングスペースの描画
void DebugEditor::DrawDockSpace(){
#ifdef USE_IMGUI
	ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockSpaceFlags);
#endif // USE_IMGUI
}

//ヒエラルキーの描画
void DebugEditor::DrawHierarchy(){
	//オブジェクトの配列がなければ
	if (!gameObjects_){
		return;
	}
#ifdef USE_IMGUI
	ImGui::Begin("オブジェクト階層");
	for (uint32_t i = 0; i < gameObjects_->size(); i++){
		const std::unique_ptr<GameObject>& currentGameObject = gameObjects_->at(i);
		//現在のゲームオブジェクトがなかった場合
		if (!currentGameObject){
			continue;
		}

		//現在のゲームオブジェクトの生ポインタを保存
		GameObject* currentGameObjectPtr = currentGameObject.get();

		//ゲームオブジェクトがNullがチェック
		if (!currentGameObjectPtr){
			return;
		}

		//ゲームオブジェクトにデバッグカメラの操作がコンポーネントであった場合
		if (currentGameObjectPtr->GetComponent<DebugCameraController>()){
			continue;
		}

		//親オブジェクトが存在した場合はスキップ
		if (currentGameObjectPtr && currentGameObjectPtr->GetParentObject()){
			continue;
		}

		//IDを追加し、同じ名前のObjectでも衝突しないようにする
		ImGui::PushID(currentGameObjectPtr);

		//選んだオブジェクトと同じかどうか
		ImGuiTreeNodeFlags isSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		//選択されたか
		if (selectedGameObject_ == currentGameObjectPtr){
			if (selectedNodePath_.empty()){
				isSelected |= ImGuiTreeNodeFlags_Selected;
			}
		}

		//存在しているかどうか
		const bool isActive = currentGameObjectPtr->IsActive();

		//フォントの色を変更
		if (!isActive){
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
		}

		//選択するテーブルの描画
		if (renamingGameObject_ == currentGameObjectPtr){
			//Renameを選んだ時だけフォーカスする
			if (requestRenameFocus_){
				ImGui::SetKeyboardFocusHere();
				requestRenameFocus_ = false;
			}

			const bool enterPressed = ImGui::InputText("##Rename", renameObjectBuffer_.data(), renameObjectBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

			//Enterで名前を確定
			if (enterPressed){
				if (renameObjectBuffer_[0] != '\0'){
					currentGameObjectPtr->SetName(renameObjectBuffer_.data());
				}

				renamingGameObject_ = nullptr;
			}
		} else{
			//行の上端
			ImVec2 rowMin = {};
			//行の下端
			ImVec2 rowMax = {};
			//マウスの位置
			ImVec2 mousePos = ImGui::GetMousePos();

			//ツリーを開く
			bool isOpen = ImGui::TreeNodeEx(currentGameObjectPtr->GetName().c_str(), isSelected);
			rowMin = ImGui::GetItemRectMin();
			rowMax = ImGui::GetItemRectMax();

			//ドラッグ元
			if (ImGui::BeginDragDropSource()){
				GameObject* dragged = currentGameObjectPtr;
				ImGui::SetDragDropPayload(kGameObjectPayloadType.c_str(), &dragged, sizeof(dragged));
				ImGui::TextUnformatted(currentGameObjectPtr->GetName().c_str());//ドラッグ中の表示
				ImGui::EndDragDropSource();
			}

			//クリックを確認
			SelectGameObjectOperation(currentGameObjectPtr);

			//ドラッグ先
			if (ImGui::BeginDragDropTarget()){
				//順番変更の操作
				bool isReorder = IsReorderObjectOperation(i, { rowMin.x,rowMin.y }, { rowMax.x,rowMax.y }, { mousePos.x,mousePos.y });
				//接続の操作
				AttachToOperation(currentGameObjectPtr, isReorder);
				ImGui::EndDragDropTarget();
			}

			//ノードを作成
			if (isOpen){
				//表示するゲームオブジェクトを探す
				for (const std::unique_ptr<GameObject>& candidate : *gameObjects_){
					//表示する候補がNullだった場合
					if (!candidate){
						continue;
					}

					//今の候補の親オブジェクトを取得
					const GameObject* candidateParent = candidate->GetParentObject();

					//今のオブジェクトと候補の親オブジェクトが一致したら
					if (currentGameObjectPtr == candidateParent){
						//矢印で開く(選択ハイライトの幅を行の右端まで伸ばす)
						ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

						//Object3dを持っているか
						Object3d* candidateObject3d = candidate->GetComponent<Object3d>();
						if (!candidateObject3d){
							continue;
						}

						//接続するパスが空じゃないか
						if (!candidateObject3d->GetAttachedNodePath().empty()){
							continue;
						}

						//ノードの取得
						Node candidateNode = {};
						//ノードの取得
						candidateNode = candidateObject3d->GetNode();

						//今描画しているGameObjetの子オブジェクト
						bool isChildOpen = ImGui::TreeNodeEx(candidate->GetName().c_str(), nodeFlags);

						//インスペクターに表示
						SelectGameObjectOperation(candidate.get());

						//接続や順番入れ替え
						if (ImGui::BeginDragDropTarget()){
							//順番変更の操作
							bool isReorder = IsReorderObjectOperation(i, { rowMin.x,rowMin.y }, { rowMax.x,rowMax.y }, { mousePos.x,mousePos.y });

							//接続の操作
							AttachToOperation(candidate.get(), isReorder);

							ImGui::EndDragDropTarget();
						}

						//親子付け解除用のドラッグ元
						if (ImGui::BeginDragDropSource()){
							DetachToOperation(candidate.get());
							ImGui::EndDragDropSource();
						}

						//子オブジェクトのツリーが開いたら
						if (isChildOpen){
							//Object3dがあった場合
							if (candidateObject3d){
								//ノードの描画
								DrawNodeTree(candidateNode, selectedNodePath_, candidate.get());
							}
							ImGui::TreePop();
						}

					}
				}

				//ノードの描画
				Object3d* object3d = currentGameObjectPtr->GetComponent<Object3d>();
				if (object3d){
					DrawNodeTree(object3d->GetNode(), object3d->GetNode().name, currentGameObjectPtr);
				}
				ImGui::TreePop();
			}
		}

		//SelectTableを描画した直後に元に戻す
		if (!isActive){
			ImGui::PopStyleColor();
		}

		//GameObjectを右クリックしたときのメニュー
		if (ImGui::BeginPopupContextItem("GameObjectContext")){

			if (ImGui::MenuItem("名前を変更")){
				BeginRename(currentGameObjectPtr);
			}

			if (ImGui::MenuItem("有効", nullptr, isActive)){
				currentGameObjectPtr->SetIsActive(!isActive);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("複製")){
				requestDuplicateGameObject_ = currentGameObjectPtr;
			}

			if (ImGui::MenuItem("削除")){
				requestDeleteGameObject_ = currentGameObjectPtr;
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	//親子付けの解除
	//空白の大きさを取得
	ImVec2 remaining = ImGui::GetContentRegionAvail();
	if (remaining.x > 0.0f && remaining.y > 0.0f){
		//空白部分を透明のボタンで埋める
		ImGui::InvisibleButton("##HierarchyEmptyDrop", remaining);

		//親子付け用のドロップ先
		if (ImGui::BeginDragDropTarget()){
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kGameObjectPayloadType.c_str());
			if (payload){
				GameObject* child = *static_cast<GameObject**>(payload->Data);
				requestDetach_ = child;
			}
			ImGui::EndDragDropTarget();
		}

		//Hierarchyの空いている場所
		if (ImGui::BeginPopupContextItem("HierarchyContext", ImGuiPopupFlags_MouseButtonRight)){
			if (ImGui::MenuItem("空のオブジェクトを作成")){
				requestCreateGameObject_ = true;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
#endif // USE_IMGUI
}

//シーンの描画
void DebugEditor::DrawScene([[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE handle){
#ifdef USE_IMGUI
	ImGui::Begin("シーン");
	//ツールバーの表示用チェックボックス
	ImGui::Checkbox("ツールバーを表示", &isGizmoToolbarVisible_);

	//テクスチャのIDを取得(GPUのハンドルから取得)
	ImTextureID textureId = reinterpret_cast<ImTextureID>(handle.ptr);

	//矩形情報を取得
	ImVec2 position = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetContentRegionAvail();
	//メンバ変数に保存
	sceneViewRectInfo_.position = { position.x,position.y };
	sceneViewRectInfo_.size = { size.x,size.y };

	//ImGuiにテクスチャを描画
	ImGui::Image(textureId, size);
	//今選択されているImGuiを判定
	isSceneViewHovered_ = ImGui::IsItemHovered();

	//Gizmoの描画
	DrawGizmo();

	//画面の左上から少し内側にツールバーを配置
	ImGui::SetCursorScreenPos(ImVec2(position.x + 8.0f, position.y + 8.0f));

	//Gizmoの切り替え用ツールバーが表示する場合
	if (isGizmoToolbarVisible_){
		//Gizmoの切り替え用ツールバー
		DrawGizmoToolbar();
	}
	ImGui::End();
#endif // USE_IMGUI
}

//Gizmoの切り替え用ツールバーの描画
void DebugEditor::DrawGizmoToolbar(){
#ifdef USE_IMGUI
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
	D3D12_GPU_DESCRIPTOR_HANDLE scaleIconHandle = textureManager_->GetSRVHandleGPU(directoryPath + "scale.png");
	D3D12_GPU_DESCRIPTOR_HANDLE rotateIconHandle = textureManager_->GetSRVHandleGPU(directoryPath + "rotate.png");
	D3D12_GPU_DESCRIPTOR_HANDLE translateIconHandle = textureManager_->GetSRVHandleGPU(directoryPath + "translate.png");

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

//Gizmoの描画
void DebugEditor::DrawGizmo(){
#ifdef USE_IMGUI
	//操作対象が存在するか
	if (!selectedGameObject_){
		return;
	}

	//デバッグカメラがあるか
	if (!debugCamera_){
		return;
	}

	ImGuizmo::SetDrawlist();

	//どれくらいの範囲で描画するか
	ImGuizmo::SetRect(
		sceneViewRectInfo_.position.x,
		sceneViewRectInfo_.position.y,
		sceneViewRectInfo_.size.x,
		sceneViewRectInfo_.size.y
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
#endif // USE_IMGUI
}

//プレビューシーンの描画
void DebugEditor::DrawPreview([[maybe_unused]] D3D12_GPU_DESCRIPTOR_HANDLE handle){
#ifdef USE_IMGUI
	ImGui::Begin("プレビュー");
	//テクスチャのIDを取得(GPUのハンドルから取得)
	ImTextureID textureId = reinterpret_cast<ImTextureID>(handle.ptr);
	//ImGuiにテクスチャを描画
	ImGui::Image(textureId, ImGui::GetContentRegionAvail());
	//今選択されているImGuiを判定
	isPreviewHovered_ = ImGui::IsItemHovered();
	ImGui::End();
#endif // USE_IMGUI
}

//インスペクターの描画
void DebugEditor::DrawInspector(){
#ifdef USE_IMGUI
	ImGui::Begin("オブジェクト詳細");
	if (selectedGameObject_){
		ImGui::SameLine();
		ImGui::TextUnformatted(selectedGameObject_->GetName().c_str());

		//有効状態
		bool isActive = selectedGameObject_->IsActive();

		//アクティブを切り替え1
		if (ImGui::Checkbox("有効", &isActive)){
			selectedGameObject_->SetIsActive(isActive);
		}

		//現在選択しているタグを取得
		const std::string currentTag = selectedGameObject_->GetTag();
		//タグのリストを取得
		const std::vector<std::string>& tagList = tagManager_->GetTagList();

		//プルダウンを表示
		if (ImGui::BeginCombo("タグ", currentTag.c_str())){
			for (const std::string& tag : tagList){
				bool isSelected = tag == currentTag;

				if (ImGui::Selectable(tag.c_str(), isSelected)){
					//選択されてタグに変更
					selectedGameObject_->SetTag(tag);
				}

				//プルダウンを開いたときに選択されているTagにフォーカスする
				if (isSelected){
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("タグ一覧に追加")){
			isTegManagerOpen_ = true;
		}

		ImGui::SeparatorText("トランスフォーム");

		//変更されたかを判定
		bool isChange = false;

		//選択しているゲームオブジェクトのTransformをコピー
		Transform transform = selectedGameObject_->GetTransform();

		//選択中のノードのパスがある場合
		if (!selectedNodePath_.empty()){
			Object3d* object3d = selectedGameObject_->GetComponent<Object3d>();
			if (object3d){
				transform = object3d->GetNodeLocalTransform(selectedNodePath_);
				inspectorEulerAngle_ = transform.GetEulerAngle();
			}
		}

		//トランスフォームのリセットボタン
		if (ImGui::Button("トランスフォームをリセット")){
			transform = {};
			isChange = true;
		}

		//スケールの切り替え
		if (ImGui::DragFloat3("拡縮", &transform.scale.x, 0.1f)){
			isChange = true;
		}
		ImGui::SameLine();
		//スケールのリセット
		if (ImGui::SmallButton("リセット##scale")){
			transform.scale = Vector3::GetOne();
			isChange = true;
		}

		//回転の切り替え(オイラー角からクォータニオンを求めてる)
		inspectorEulerAngle_ = transform.GetEulerAngle();
		if (ImGui::DragFloat3("回転", &inspectorEulerAngle_.x, 0.1f)){
			transform.rotate = Quaternion::EulerAngleToQuaternion(inspectorEulerAngle_);
			isChange = true;
		}
		ImGui::SameLine();
		//回転のリセット
		if (ImGui::SmallButton("リセット##rotate")){
			transform.rotate = Quaternion::IdentityQuaternion();
			isChange = true;
		}


		//平行移動成分の切り替え
		if (ImGui::DragFloat3("平行移動", &transform.translate.x, 0.1f)){
			isChange = true;
		}
		ImGui::SameLine();
		//平行成分のリセット
		if (ImGui::SmallButton("リセット##translate")){
			transform.translate = { 0.0f,0.0f,0.0f };
			isChange = true;
		}

		//変更されている場合
		if (isChange){
			//最終的な結果を元のTransformに反映
			if (!selectedNodePath_.empty()){
				Object3d* object3d = selectedGameObject_->GetComponent<Object3d>();
				if (object3d){
					object3d->SetNodeLocalTransform(selectedNodePath_, transform);
				}
			} else{
				selectedGameObject_->GetTransform() = transform;
			}
		}

		//各コンポーネントを操作
		DrawPropertyTable(selectedGameObject_);

	} else{
		ImGui::TextDisabled("オブジェクトを選択していない");
	}
	ImGui::End();
#endif // USE_IMGUI
}

//各コンポーネントのプロパティ一覧
void DebugEditor::DrawPropertyTable([[maybe_unused]] GameObject* selectGameObject){
#ifdef USE_IMGUI
	//各コンポーネントを取得
	const std::vector<std::unique_ptr<Component>>& components = selectGameObject->GetAllComponents();
	//コンポーネントがなければ
	if (components.empty()){
		return;
	}

	//各コンポーネントにアクセス
	for (uint32_t i = 0; i < components.size(); i++){
		ImGui::PushID(i);

		//コンポーネントを取得
		Component* component = components[i].get();
		//Nullかどうか確認
		if (!component){
			ImGui::PopID();
			continue;
		}

		//コンポーネントを操作できる
		DrawProperties(component);

		ImGui::PopID();
	}
#endif // USE_IMGUI
}

//1つのコンポーネントのプロパティ一覧
void DebugEditor::DrawProperties([[maybe_unused]] Component* component){
#ifdef USE_IMGUI
	//headerを作成
	if (ImGui::CollapsingHeader(component->GetTypeName().c_str())){
		//インデントをつける
		ImGui::Indent();
		//登録されているプロパティ一覧を取得
		PropertyRegistry registry;
		component->RegisterProperties(&registry);
		std::vector<Property> properties = registry.GetProperties();

		//グループのテーブルを表示するか
		bool isGroupTableVisible = true;

		//レジストリを使ってIｍGuiを描画
		for (Property property : properties){
			//Groupの開始を取得
			inspector::GroupBegin* groupBegin = std::get_if<inspector::GroupBegin>(&property);
			//Groupの終了を取得
			inspector::GroupEnd* groupEnd = std::get_if<inspector::GroupEnd>(&property);
			//浮動小数のプロパティを取得
			inspector::FloatProperty* floatProperty = std::get_if<inspector::FloatProperty>(&property);
			//フラグのプロパティを取得
			inspector::BoolProperty* boolProperty = std::get_if<inspector::BoolProperty>(&property);
			//整数のプロパティを取得
			inspector::IntProperty* intProperty = std::get_if<inspector::IntProperty>(&property);
			//Vector2のプロパティを取得
			inspector::Vector2Property* vector2Property = std::get_if<inspector::Vector2Property>(&property);
			//Vector3のプロパティを取得
			inspector::Vector3Property* vector3Property = std::get_if<inspector::Vector3Property>(&property);
			//Vector4のプロパティを取得
			inspector::Vector4Property* vector4Property = std::get_if<inspector::Vector4Property>(&property);
			//Colorのプロパティを取得
			inspector::ColorProperty* colorProperty = std::get_if<inspector::ColorProperty>(&property);
			//Stringのプロパティを取得
			inspector::StringProperty* stringProperty = std::get_if<inspector::StringProperty>(&property);
			//Enumのプロパティを取得
			inspector::EnumProperty* enumProperty = std::get_if<inspector::EnumProperty>(&property);
			//グループの開始だった場合
			if (groupBegin){
				isGroupTableVisible = ImGui::CollapsingHeader(groupBegin->displayName.c_str());
				continue;
			}

			//グループの終了だった場合
			if (groupEnd){
				isGroupTableVisible = true;
				continue;
			}

			//表示するフラグがfalseだった場合
			if (!isGroupTableVisible){
				continue;
			}

			//各項目のIｍGuiを表示
			if (floatProperty){
				float decimal = floatProperty->getter();
				if (ImGui::DragFloat(floatProperty->displayName.c_str(), &decimal, floatProperty->dragSpeed)){
					floatProperty->setter(decimal);
				}
			} else if (boolProperty){
				bool frag = boolProperty->getter();
				if (ImGui::Checkbox(boolProperty->displayName.c_str(), &frag)){
					boolProperty->setter(frag);
				}
			} else if (intProperty){
				int32_t digit = intProperty->getter();
				if (ImGui::DragInt(intProperty->displayName.c_str(), &digit, intProperty->dragSpeed)){
					intProperty->setter(digit);
				}
			} else if (vector2Property){
				Vector2 vector2 = vector2Property->getter();
				if (ImGui::DragFloat2(vector2Property->displayName.c_str(), &vector2.x, vector2Property->dragSpeed)){
					vector2Property->setter(vector2);
				}
			} else if (vector3Property){
				Vector3 vector3 = vector3Property->getter();
				if (ImGui::DragFloat3(vector3Property->displayName.c_str(), &vector3.x, vector3Property->dragSpeed)){
					vector3Property->setter(vector3);
				}
			} else if (vector4Property){
				Vector4 vector4 = vector4Property->getter();
				if (ImGui::DragFloat4(vector4Property->displayName.c_str(), &vector4.x, vector4Property->dragSpeed)){
					vector4Property->setter(vector4);
				}
			} else if (colorProperty){
				Color color = colorProperty->getter();
				if (ImGui::ColorEdit4(colorProperty->displayName.c_str(), &color.r)){
					colorProperty->setter(color);
				}
			} else if (stringProperty){
				std::string text = stringProperty->getter();
				std::array<char, 256> buffer = {};

				//終端文字用に一つ残す
				const size_t copyLength = std::min(text.size(), buffer.size() - 1);

				//テキストの内容をBufferにコピー
				std::copy_n(text.data(), copyLength, buffer.data());

				//テキストを操作
				if (ImGui::InputText(stringProperty->displayName.c_str(), buffer.data(), buffer.size())){
					stringProperty->setter(static_cast<std::string>(buffer.data()));
				}
			} else if (enumProperty){
				uint32_t selectOption = enumProperty->getter();
				if (ImGui::BeginCombo(enumProperty->displayName.c_str(), enumProperty->options[selectOption].c_str())){
					for (uint32_t i = 0; i < static_cast<uint32_t>(enumProperty->options.size()); i++){
						//選択されたら
						bool isSelect = (i == selectOption);

						//オプションが選択されたら
						if (ImGui::Selectable(enumProperty->options[i].c_str(), isSelect)){
							selectOption = i;
							enumProperty->setter(selectOption);
						}

						if (isSelect){
							ImGui::SetItemDefaultFocus();//次に開いたときにフォーカスされる
						}
					}
					ImGui::EndCombo();
				}
			}
		}

		//もし有効じゃなければ
		if (!component->IsEnabled()){
			return;
		}

		//インデントを解除
		ImGui::Unindent();
	}
#endif // USE_IMGUI
}

//タグの管理の描画
void DebugEditor::DrawTagManager(){
#ifdef USE_IMGUI


	//タグの管理がNullかどうか
	if (!tagManager_){
		return;
	}

	if (!ImGui::Begin("タグの管理", &isTegManagerOpen_)){
		ImGui::End();
		return;
	}

	//新しいタグの入力欄
	ImGui::SetNextItemWidth(200.0f);

	const bool pressedEnter = ImGui::InputText("##新しいタグ名", newTagNameBuffer_.data(), newTagNameBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue);

	ImGui::SameLine();

	const bool pressedAdd = ImGui::Button("追加");

	//EnterまたはAddボタンで追加
	if (pressedEnter || pressedAdd){
		const std::string newTagName = newTagNameBuffer_.data();

		if (!newTagName.empty()){
			tagManager_->AddTag(newTagName);

			//入力欄を空に戻す
			newTagNameBuffer_.fill('\0');
		}
	}
	ImGui::Separator();

	//名前変更のポップアップを開く要求
	bool isOpenRenamePopup = false;
	//削除のポップアップを開く要求
	bool isOpenDeletePopup = false;

	//タグ一覧
	const std::vector<std::string>& tagList = tagManager_->GetTagList();

	for (const std::string& tag : tagList){
		ImGui::PushID(tag.c_str());

		ImGui::Selectable(tag.c_str());

		//タグを右クリック
		if (ImGui::BeginPopupContextItem("TagContexMenu")){
			const bool isDefaultTag = tag == TagManager::kDefaultTagName;

			ImGui::BeginDisabled(isDefaultTag);

			if (ImGui::MenuItem("名前変更")){
				renameTargetTag_ = tag;
				renameTagBuffer_.fill('\0');

				const size_t copySize = std::min(tag.size(), renameTagBuffer_.size() - 1);

				std::copy_n(tag.data(), copySize, renameTagBuffer_.data());

				isOpenRenamePopup = true;
			}

			if (ImGui::MenuItem("削除")){
				deleteTargetTag_ = tag;
				isOpenDeletePopup = true;
			}

			ImGui::EndDisabled();
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	//同じID階層からポップアップを開く
	//名前変更
	if (isOpenRenamePopup){
		ImGui::OpenPopup("タグ名を変更");
	}

	//削除
	if (isOpenDeletePopup){
		ImGui::OpenPopup("タグを削除");
	}

	//名前変更ポップアップ
	if (ImGui::BeginPopupModal("タグ名を変更", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text("Rename \"%s\"", renameTargetTag_.c_str());

		//文字を入力時にエンターを押したか
		const bool pressEnter = ImGui::InputText("新しい名前", renameTagBuffer_.data(), renameTagBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		//ボタンを押したか

		const std::string newTagName = renameTagBuffer_.data();

		const bool cannotRename = newTagName.empty() || newTagName == renameTargetTag_;

		ImGui::BeginDisabled(cannotRename);
		const bool pressRename = ImGui::Button("名前変更");

		if ((pressEnter || pressRename) && !cannotRename){
			requestRenameTag_ = {
				.oldTag = renameTargetTag_,
				.newTag = newTagName
			};

			renameTargetTag_.clear();
			renameTagBuffer_.fill('\0');

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndDisabled();

		ImGui::SameLine();

		if (ImGui::Button("キャンセル")){
			renameTargetTag_.clear();
			renameTagBuffer_.fill('\0');

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//削除確認ポップアップ
	if (ImGui::BeginPopupModal("タグを削除", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text("Delete \"%s\"?", deleteTargetTag_.c_str());

		ImGui::TextUnformatted("このタグを使用しているオブジェクトは、タグなしに変更されます。");

		if (ImGui::Button("削除")){
			requestDeleteTag_ = deleteTargetTag_;

			deleteTargetTag_.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("キャンセル")){
			deleteTargetTag_.clear();

			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::End();
#endif // USE_IMGUI
}

//名前変更を開始
void DebugEditor::BeginRename(GameObject* gameObject){
	//ゲームオブジェクトがなければ
	if (!gameObject){
		return;
	}

	renamingGameObject_ = gameObject;
	requestRenameFocus_ = true;

	//バッファを初期化
	renameObjectBuffer_.fill('\0');

	//今の名前を取得
	const std::string& name = gameObject->GetName();

	const std::size_t copyLength = std::min(name.size(), renameObjectBuffer_.size() - 1);

	std::copy_n(
		name.data(),
		copyLength,
		renameObjectBuffer_.data()
	);
}

//ノードのツリーを描画
void DebugEditor::DrawNodeTree([[maybe_unused]] const Node& node, [[maybe_unused]] const std::string& parentPath, [[maybe_unused]] GameObject* targetObject){
#ifdef USE_IMGUI
	//現在のパス
	std::string currentPath = parentPath;

	//矢印で開く(選択ハイライトの幅を行の右端まで伸ばす)
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	//子ノードがなければ
	if (node.children.empty()){
		//末端ノードにする
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	//選択されていたらハイライトをつける
	if (selectedGameObject_ == targetObject){
		if (selectedNodePath_ == currentPath){
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}
	}

	//ツリーを開く
	bool isOpen = ImGui::TreeNodeEx(static_cast<const void*>(&node), nodeFlags, "%s", node.name.c_str());

	//子ノードを開く前に確認する(矢印をクリックしてない場合)
	bool isSelectedObject = SelectGameObjectOperation(targetObject);
	if (isSelectedObject){
		//このNodeのパスを選択中のパスとして取得
		selectedNodePath_ = currentPath;
	}

	//親子付け用のドロップ先
	if (ImGui::BeginDragDropTarget()){
		//接続
		AttachToOperation(targetObject, false, currentPath);
		ImGui::EndDragDropTarget();
	}

	//接続されているゲームオブジェクト一覧
	std::vector<GameObject*>attachedGameObjects;

	//このノードに接続されたGameObjectがないか探索
	for (const std::unique_ptr<GameObject>& gameObject : *gameObjects_){
		//親オブジェクトが存在するか
		const GameObject* parentObject = gameObject->GetParentObject();
		if (!parentObject){
			continue;
		}

		//対象と親が一致しているか
		if (targetObject == parentObject){
			Object3d* object3d = gameObject->GetComponent<Object3d>();
			if (object3d){
				//現在のパスと接続されているノードのパスが一致しているか
				if (currentPath == object3d->GetAttachedNodePath()){
					attachedGameObjects.push_back(gameObject.get());
				}
			}
		}
	}

	//ツリーが開いていたら
	if (isOpen){
		//子ノードを描画
		for (const Node& child : node.children){
			DrawNodeTree(child, currentPath + "/" + child.name, targetObject);
		}

		//接続されているゲームオブジェクトを描画
		for (GameObject* attachedGameObject : attachedGameObjects){
			//Object3dを取得
			Object3d* attachedObject3d = attachedGameObject->GetComponent<Object3d>();
			if (!attachedObject3d){
				continue;
			}

			//アタッチしているオブジェクトのフラグ
			ImGuiTreeNodeFlags attachedFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

			//選択されたらハイライトを出す
			if (selectedGameObject_ == attachedGameObject){
				if (selectedNodePath_.empty()){
					attachedFlags |= ImGuiTreeNodeFlags_Selected;
				}
			}

			//接続されているGameObjectのツリーを開く
			bool isAttachedOpen = ImGui::TreeNodeEx(
				static_cast<const void*>(attachedGameObject), attachedFlags,
				"%s",
				attachedGameObject->GetName().c_str()
			);

			//オブジェクトを選択
			SelectGameObjectOperation(attachedGameObject);

			//親子付け解除用のドラッグ元
			if (ImGui::BeginDragDropSource()){
				DetachToOperation(attachedGameObject);
				ImGui::EndDragDropSource();
			}

			//ノードを取得
			const Node& attachedNode = attachedObject3d->GetNode();

			if (isAttachedOpen){
				//ツリーに描画
				DrawNodeTree(attachedNode, attachedNode.name, attachedGameObject);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}

//オブジェクト階層で順番入れ替えの操作
bool DebugEditor::IsReorderObjectOperation([[maybe_unused]] uint32_t currentIndex, [[maybe_unused]] const Vector2& rowMin, [[maybe_unused]] const Vector2& rowMax, [[maybe_unused]] const Vector2& mousePos){
#ifdef USE_IMGUI
	//行の高さ
	float rowHight = rowMax.y - rowMin.y;
	//上端からのマウスの位置
	float rowMinToMousePos = mousePos.y - rowMin.y;
	//並び替えを行うか
	bool isReorder = rowMinToMousePos < rowHight * 0.25f;//距離が行の高さの25%未満なら

	//ペイロード取得の際の設定(フラグ)
	ImGuiDragDropFlags flags = ImGuiDragDropFlags_AcceptBeforeDelivery;
	//並び替えを行う先の表示
	if (isReorder){
		//並び替えをする際はオブジェクトごとに枠を表示しないようにする
		flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
		//上端に横線を表示
		ImGui::GetWindowDrawList()->AddLine({ rowMin.x,rowMin.y }, { rowMax.x,rowMin.y }, IM_COL32(80, 170, 255, 255), 2.0f);
	}

	//ペイロードを取得
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kGameObjectPayloadType.c_str(), flags);
	//payloadがnullじゃなければ
	if (payload){
		if (payload->IsDelivery()){
			//並び替えを行うか
			if (isReorder){
				//並び替えを行う
				GameObject* dropObject = *static_cast<GameObject**>(payload->Data);
				//ドラッグしたインデックスを取得
				for (uint32_t draggedIndex = 0; draggedIndex < gameObjects_->size(); draggedIndex++){
					if (gameObjects_->at(draggedIndex).get() == dropObject){
						draggedIndex_ = draggedIndex;
						break;
					}
				}
				dropTargetIndex_ = currentIndex;

				//移動前と移動後のインデックスが違かったら
				if (draggedIndex_ != dropTargetIndex_){
					//ゲームオブジェクトの移動リクエストを要求
					requestMoveGameObject_ = true;
				}
			}
		}
	}
	return isReorder;
#endif // USE_IMGUI
}

//オブジェクト階層で接続させる操作
void DebugEditor::AttachToOperation([[maybe_unused]] GameObject* parentObject, [[maybe_unused]] bool isReorder, [[maybe_unused]] const std::string& nodePath){
#ifdef USE_IMGUI
	//順序入れ替えが行われていなければ
	if (isReorder){
		return;
	}

	//ペイロード取得の際の設定(フラグ)
	ImGuiDragDropFlags flags = ImGuiDragDropFlags_AcceptBeforeDelivery;

	//ペイロードを取得
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kGameObjectPayloadType.c_str(), flags);
	//payloadがnullじゃなければ
	if (payload){
		if (payload->IsDelivery()){
			//親子付けを要求
			GameObject* child = *static_cast<GameObject**>(payload->Data);
			requestAttachTo_ = ParentRequest{ child,parentObject,nodePath };
		}
	}
#endif // USE_IMGUI
}

//オブジェクト階層で接続を解除する操作
void DebugEditor::DetachToOperation(GameObject* attachedObject){
	GameObject* dragged = attachedObject;
	ImGui::SetDragDropPayload(kGameObjectPayloadType.c_str(), &dragged, sizeof(dragged));
	ImGui::TextUnformatted(attachedObject->GetName().c_str());//ドラッグ中の表示
}

//オブジェクトの階層で選択する操作
bool DebugEditor::SelectGameObjectOperation(GameObject* selectGameObject){
	//表示した子オブジェクトの行のクリックを確認
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
		SelectGameObject(selectGameObject);
		return true;
	}
	return false;
}
