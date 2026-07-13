#include "DebugEditor.h"
#include "GameObject.h"
#include "ImGuiManager.h"

//コンストラクタ
DebugEditor::DebugEditor(){
}

//デストラクタ
DebugEditor::~DebugEditor(){
}

//初期化
void DebugEditor::Initialize(){
	//選択するオブジェクトの初期化
	selectedGameObject_ = nullptr;
	gameObjects_ = nullptr;
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
#endif // USE_IMGUI
}

//ゲームオブジェクト一覧の設定
void DebugEditor::SetGameObjects(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	gameObjects_ = &gameObjects;
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
			GameObject* object = gameObject.get();

			ImGui::PushID(object);

			//選んだオブジェクトと同じかどうか
			const bool isSelected = selectedGameObject_ == object;

			//存在しているかどうか
			const bool isActive = object->IsActive();

			//存在していなかったら
			if (!isActive){
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
			}

			if (ImGui::Selectable(object->GetName().c_str(), isSelected)){
				selectedGameObject_ = object;
			}

			if (!isActive){
				ImGui::PopStyleColor();
			}

			ImGui::PopID();
		}
		ImGui::End();
	}
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
