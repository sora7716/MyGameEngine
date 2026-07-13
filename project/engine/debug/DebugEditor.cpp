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
	ImGui::TextUnformatted("GameObjects");
	if (gameObjects_){
		for (const std::unique_ptr<GameObject>& gameObject : *gameObjects_){
			//ゲームオブジェクトがなかった場合
			if (!gameObject){
				continue;
			}

			//ゲームオブジェクトの生ポインタを保存
			GameObject* object = gameObject.get();

			//選んだオブジェクトと同じかどうか
			const bool isSelected = selectedGameObject_ == object;

			if (ImGui::Selectable(object->GetName().c_str(), isSelected)){
				selectedGameObject_ = object;
			}
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
		ImGui::TextUnformatted("Object selected");
		Transform& transform = selectedGameObject_->GetTransform();

		ImGui::DragFloat3("translate", &transform.translate.x, 0.1f);
		if (ImGui::DragFloat3("rotate", &transform.eulerAngle.x, 0.1f)){
			transform.quaternion = Quaternion::MakeQuaternionForEulerAngle(transform.eulerAngle);
		};
		ImGui::DragFloat3("scale", &transform.scale.x, 0.1f);
	} else{
		ImGui::TextDisabled("No object selected");
	}
	ImGui::End();
#endif // USE_IMGUI
}
