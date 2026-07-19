#include "DebugEditor.h"
#include "GameObject.h"
#include "ImGuiManager.h"
#include "TagManager.h"

//コンストラクタ
DebugEditor::DebugEditor(){
}

//デストラクタ
DebugEditor::~DebugEditor(){
}

//初期化
void DebugEditor::Initialize(TagManager* tagManager){
	//Tagの管理のNULLチェック
	assert(tagManager);
	tagManager_ = tagManager;
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
void DebugEditor::Draw(){
#ifdef USE_IMGUI
	//ドッキングスペースの描画
	DrawDockSpace();
	//ヒエラルキーの描画
	DrawHierarchy();
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

//GameObjectを選択
void DebugEditor::SelectGameObject(GameObject* gameObject){
	selectedGameObject_ = gameObject;
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
	ImGui::Begin("Hierarchy");
	//ImGui::TextUnformatted("GameObjects");
	if (gameObjects_){
		for (const std::unique_ptr<GameObject>& gameObject : *gameObjects_){
			//ゲームオブジェクトがなかった場合
			if (!gameObject){
				continue;
			}

			//ゲームオブジェクトの生ポインタを保存
			GameObject* gameObjectPtr = gameObject.get();

			//IDを追加し、同じ名前のObjectでも衝突しないようにする
			ImGui::PushID(gameObjectPtr);

			//選んだオブジェクトと同じかどうか
			const bool isSelected = selectedGameObject_ == gameObjectPtr;

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

				const bool enterPressed = ImGui::InputText("##Rename", renameBuffer_.data(), renameBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

				//Enterで名前を確定
				if (enterPressed){
					if (renameBuffer_[0] != '\0'){
						gameObjectPtr->SetName(renameBuffer_.data());
					}

					renamingGameObject_ = nullptr;
				}
			} else{
				if (ImGui::Selectable(gameObjectPtr->GetName().c_str(), isSelected)){
					selectedGameObject_ = gameObjectPtr;
				}
			}

			//SelectTableを描画した直後に元に戻す
			if (!isActive){
				ImGui::PopStyleColor();
			}

			//GameObjectを右クリックしたときのメニュー
			if (ImGui::BeginPopupContextItem("GameObjectContext")){

				if (ImGui::MenuItem("Rename")){
					BeginRename(gameObjectPtr);
				}

				if (ImGui::MenuItem("active", nullptr, isActive)){
					gameObjectPtr->SetIsActive(!isActive);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Duplicate")){
					requestDuplicateGameObject_ = gameObjectPtr;
				}

				if (ImGui::MenuItem("Delete")){
					requestDeleteGameObject_ = gameObjectPtr;
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		//Hierarchyの空いている場所を右クリック
		if (ImGui::BeginPopupContextWindow("HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)){
			if (ImGui::MenuItem("Create Empty")){
				requestCreateGameObject_ = true;
			}
			ImGui::EndPopup();
		}

	}
	ImGui::End();
#endif // USE_IMGUI
}

//インスペクターの描画
void DebugEditor::DrawInspector(){
#ifdef USE_IMGUI
	ImGui::Begin("Inspector");
	if (selectedGameObject_){
		ImGui::SameLine();
		ImGui::TextUnformatted(selectedGameObject_->GetName().c_str());

		//有効状態
		bool isActive = selectedGameObject_->IsActive();

		//アクティブを切り替え1
		if (ImGui::Checkbox("active", &isActive)){
			selectedGameObject_->SetIsActive(isActive);
		}

		//現在選択しているタグを取得
		const std::string currentTag = selectedGameObject_->GetTag();
		//タグのリストを取得
		const std::vector<std::string>& tagList = tagManager_->GetTagList();

		//プルダウンを表示
		if (ImGui::BeginCombo("tag", currentTag.c_str())){
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
		if (ImGui::Button("Edit Tags")){
			isTegManagerOpen_ = true;
		}

		ImGui::SeparatorText("transform");

		Transform& transform = selectedGameObject_->GetTransform();

		//トランスフォームのリセットボタン
		if (ImGui::Button("Reset Transform")){
			transform.Initialize();
		}

		//スケールの切り替え
		ImGui::DragFloat3("scale", &transform.scale.x, 0.1f);
		ImGui::SameLine();
		//スケールのリセット
		if (ImGui::SmallButton("Reset##scale")){
			transform.scale = Vector3::MakeAllOne();
		}

		//回転の切り替え(オイラー角からクォータニオンを求めてる)
		if (ImGui::DragFloat3("rotate", &transform.eulerAngle.x, 0.1f)){
			transform.quaternion = Quaternion::MakeQuaternionForEulerAngle(transform.eulerAngle);
		}
		ImGui::SameLine();
		//回転のリセット
		if (ImGui::SmallButton("Reset##rotate")){
			transform.eulerAngle = { 0.0f,0.0f,0.0f };
			transform.quaternion = Quaternion::IdentityQuaternion();
		}


		//平行移動成分の切り替え
		ImGui::DragFloat3("translate", &transform.translate.x, 0.1f);
		ImGui::SameLine();
		//平行成分のリセット
		if (ImGui::SmallButton("Reset##translate")){
			transform.translate = { 0.0f,0.0f,0.0f };
		}

	} else{
		ImGui::TextDisabled("No object selected");
	}
	ImGui::End();
#endif // USE_IMGUI
}

//タグの管理の描画
void DebugEditor::DrawTagManager(){
	//タグの管理がNullかどうか
	if (!tagManager_){
		return;
	}

	if (!ImGui::Begin("Tag Manager", &isTegManagerOpen_)){
		ImGui::End();
		return;
	}

	//新しいタグの入力欄
	ImGui::SetNextItemWidth(200.0f);

	const bool pressedEnter = ImGui::InputText("##NewTagName", newTagNameBuffer_.data(), newTagNameBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue);

	ImGui::SameLine();

	const bool pressedAdd = ImGui::Button("Add");

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

	//タグ一覧
	const std::vector<std::string>& tagList = tagManager_->GetTagList();

	for (const std::string& tag : tagList){
		ImGui::PushID(tag.c_str());

		ImGui::Selectable(tag.c_str());

		//タグを右クリック
		if (ImGui::BeginPopupContextItem("TagContexMenu")){
			const bool isDefaultTag = tag == TagManager::kDefaultTagName;

			ImGui::BeginDisabled(isDefaultTag);

			if (ImGui::MenuItem("Rename")){
				renameTargetTag_ = tag;
				renameBuffer_.fill('\0');

				const size_t copySize = std::min(tag.size(), renameBuffer_.size() - 1);

				std::copy_n(tag.data(), copySize, renameBuffer_.data());

				ImGui::OpenPopup("Rename Tag");
			}

			if (ImGui::MenuItem("Delete")){
				deleteTargetTag_ = tag;
				ImGui::OpenPopup("Delete Tag");
			}

			ImGui::EndDisabled();
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	//名前変更ポップアップ
	if (ImGui::BeginPopupModal("Rename Tag", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text("Rename \"%s\"", renameTargetTag_.c_str());

		ImGui::InputText("New Name", renameBuffer_.data(), renameBuffer_.size());

		const std::string newTagName = renameBuffer_.data();

		const bool cannotRename = newTagName.empty() || newTagName == renameTargetTag_;

		ImGui::BeginDisabled(cannotRename);

		if (ImGui::Button("Rename")){
			tagManager_->RenameTag(renameTargetTag_, newTagName);

			renameTargetTag_.clear();
			renameBuffer_.fill('\0');

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndDisabled();

		ImGui::SameLine();

		if (ImGui::Button("Cancel")){
			renameTargetTag_.clear();
			renameBuffer_.fill('\0');

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//削除確認ポップアップ
	if (ImGui::BeginPopupModal("Delete Tag", nullptr, ImGuiWindowFlags_AlwaysAutoResize)){
		ImGui::Text("Delete \"%s\"?", deleteTargetTag_.c_str());

		ImGui::TextUnformatted("Objects using this tag should be changed to Untagges.");

		if (ImGui::Button("Delete")){
			tagManager_->RemoveTag(deleteTargetTag_);

			deleteTargetTag_.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel")){
			deleteTargetTag_.clear();

			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::End();
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
	renameBuffer_.fill('\0');

	//今の名前を取得
	const std::string& name = gameObject->GetName();

	const std::size_t copyLength = std::min(name.size(), renameBuffer_.size() - 1);

	std::copy_n(
		name.data(),
		copyLength,
		renameBuffer_.data()
	);
}