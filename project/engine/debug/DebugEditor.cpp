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
#include <cassert>

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
void DebugEditor::Draw(D3D12_GPU_DESCRIPTOR_HANDLE sceneHandle, D3D12_GPU_DESCRIPTOR_HANDLE previewHandle){
	(void)sceneHandle;
	(void)previewHandle;
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
GameObject* DebugEditor::ConsumeDeleteRequest(){
	GameObject* target = requestDeleteGameObject_;

	requestDeleteGameObject_ = nullptr;

	//選択中のオブジェクトを削除する場合
	if (selectedGameObject_ == target){
		selectedGameObject_ = nullptr;
	}

	return target;
}

//複製要求を取得
GameObject* DebugEditor::ConsumeDuplicateRequest(){
	GameObject* target = requestDuplicateGameObject_;

	requestDuplicateGameObject_ = nullptr;

	return target;
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
#ifdef USE_IMGUI
	ImGui::Begin("オブジェクト階層");
	if (gameObjects_){
		for (uint32_t i = 0; i < gameObjects_->size(); i++){
			const std::unique_ptr<GameObject>& gameObject = gameObjects_->at(i);
			//ゲームオブジェクトがなかった場合
			if (!gameObject){
				continue;
			}

			//ゲームオブジェクトの生ポインタを保存
			GameObject* gameObjectPtr = gameObject.get();

			//ゲームオブジェクトにデバッグカメラの操作がコンポーネントであった場合
			if (gameObjectPtr->GetComponent<DebugCameraController>()){
				continue;
			}

			//親オブジェクトが存在した場合はスキップ
			Object3d* object3d = gameObjectPtr->GetComponent<Object3d>();
			if (object3d && object3d->GetParentObject3d()){
				continue;
			}

			//IDを追加し、同じ名前のObjectでも衝突しないようにする
			ImGui::PushID(gameObjectPtr);

			//選んだオブジェクトと同じかどうか
			ImGuiTreeNodeFlags isSelected = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

			if (selectedGameObject_ == gameObjectPtr){
				if (selectedNodePath_.empty()){
					isSelected |= ImGuiTreeNodeFlags_Selected;
				}
			}

			//存在しているかどうか
			const bool isActive = gameObjectPtr->IsActive();

			//フォントの色を変更
			if (!isActive){
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			}

			//選択するテーブルの描画
			if (renamingGameObject_ == gameObjectPtr){
				//Renameを選んだ時だけフォーカスする
				if (requestRenameFocus_){
					ImGui::SetKeyboardFocusHere();
					requestRenameFocus_ = false;
				}

				const bool enterPressed = ImGui::InputText("##Rename", renameObjectBuffer_.data(), renameObjectBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

				//Enterで名前を確定
				if (enterPressed){
					if (renameObjectBuffer_[0] != '\0'){
						gameObjectPtr->SetName(renameObjectBuffer_.data());
					}

					renamingGameObject_ = nullptr;
				}
			} else{
				//ツリーを開く
				bool isOpen = ImGui::TreeNodeEx(gameObjectPtr->GetName().c_str(), isSelected);

				//クリックを確認
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
					SelectGameObject(gameObjectPtr);
				}

				if (isOpen){
					//ノードの描画
					if (object3d){
						DrawNodeTree(object3d->GetNode(), object3d->GetNode().name, object3d);
					}

					ImGui::TreePop();
				}

				//ドラッグ元
				if (ImGui::BeginDragDropSource()){
					ImGui::SetDragDropPayload(kGameObjectPayloadType.c_str(), &i, sizeof(i));
					ImGui::EndDragDropSource();
				}

				//ドラッグ先
				if (ImGui::BeginDragDropTarget()){
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kGameObjectPayloadType.c_str());

					//payloadがnullじゃなければ
					if (payload){
						draggedIndex_ = *static_cast<uint32_t*>(payload->Data);
						dropTargetIndex_ = i;

						//移動前と移動後のインデックスが違かったら
						if (draggedIndex_ != dropTargetIndex_){
							//ゲームオブジェクトの移動リクエストを要求
							requestMoveGameObject_ = true;
						}
					}

					ImGui::EndDragDropTarget();
				}
			}

			//SelectTableを描画した直後に元に戻す
			if (!isActive){
				ImGui::PopStyleColor();
			}

			//GameObjectを右クリックしたときのメニュー
			if (ImGui::BeginPopupContextItem("GameObjectContext")){

				if (ImGui::MenuItem("名前を変更")){
					BeginRename(gameObjectPtr);
				}

				if (ImGui::MenuItem("有効", nullptr, isActive)){
					gameObjectPtr->SetIsActive(!isActive);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("複製")){
					requestDuplicateGameObject_ = gameObjectPtr;
				}

				if (ImGui::MenuItem("削除")){
					requestDeleteGameObject_ = gameObjectPtr;
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		//Hierarchyの空いている場所を右クリック
		if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)){
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
void DebugEditor::DrawScene(D3D12_GPU_DESCRIPTOR_HANDLE handle){
	(void)handle;
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
	Matrix4x4 worldMatrix = matrixUtility::MakeAffineMatrix(selectedGameObject_->GetTransform());

	//実際に動かす
	bool isChangedMatrix = ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], static_cast<ImGuizmo::OPERATION>(gizmoTool_), ImGuizmo::WORLD, &worldMatrix.m[0][0]);

	//行列が変更されたか
	if (isChangedMatrix){
		//ワールド行列からトランスフォームに分解
		Transform transform = matrixUtility::DecomposeMatrix(worldMatrix, selectedGameObject_->GetTransform().scale);
		//Transformを設定
		selectedGameObject_->GetTransform() = transform;
	}
#endif // USE_IMGUI
}

//プレビューシーンの描画
void DebugEditor::DrawPreview(D3D12_GPU_DESCRIPTOR_HANDLE handle){
	(void)handle;
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

	} else{
		ImGui::TextDisabled("オブジェクトを選択していない");
	}
	ImGui::End();
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
void DebugEditor::DrawNodeTree(const Node& node, const std::string& parentPath, Object3d* targetObject){
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
	if (selectedGameObject_ == targetObject->GetOwner()){
		if (selectedNodePath_ == currentPath){
			nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}
	}

	//ツリーを開く
	bool isOpen = ImGui::TreeNodeEx(static_cast<const void*>(&node), nodeFlags, "%s", node.name.c_str());

	//子ノードを開く前に確認する(矢印をクリックしてない場合)
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
		SelectGameObject(targetObject->GetOwner());
		//このNodeのパスを選択中のパスとして取得
		selectedNodePath_ = currentPath;
	}

	//接続されているゲームオブジェクト一覧
	std::vector<GameObject*>attachedGameObjects;

	//このノードに接続されたGameObjectがないか探索
	for (const std::unique_ptr<GameObject>& gameObject : *gameObjects_){
		//Object3dが存在するか
		Object3d* object3d = gameObject->GetComponent<Object3d>();
		if (!object3d){
			continue;
		}

		//親オブジェクトが存在するか
		const Object3d* parentObject = object3d->GetParentObject3d();
		if (!parentObject){
			continue;
		}
		//対象のObject3dと親のObject3dが一致しているか
		if (targetObject == parentObject){
			//現在のパスが親のパスと一致しているか
			std::string parentNodePath = object3d->GetParentNodePath();
			if (currentPath == parentNodePath){
				//接続されているゲームオブジェクト一覧に追加
				attachedGameObjects.push_back(gameObject.get());
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
				attachedGameObject->GetName().c_str());

			//表示した子オブジェクトの行のクリックを確認
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
				SelectGameObject(attachedGameObject);
			}

			//ノードを取得
			const Node& attachedNode = attachedObject3d->GetNode();

			if (isAttachedOpen){
				//ツリーに描画
				DrawNodeTree(attachedNode, attachedNode.name, attachedObject3d);
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
#endif // USE_IMGUI
}
