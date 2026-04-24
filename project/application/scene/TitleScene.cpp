#include "TitleScene.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/scene/SceneManager.h"
#include "engine/2d/Text.h"
#include "engine/base/Core.h"

//コンストラクタ
TitleScene::TitleScene() {
};

//デストラクタ
TitleScene::~TitleScene() {
};

//初期化
void TitleScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	BaseScene::Initialize(sceneContext);
	camera_ = sceneContext_.cameraManager->FindCamera("titleCamera");

	//タイトル名
	titleName_ = std::make_unique<Text>();
	titleName_->Initialize(sceneContext_.object2dCommon,"titleName");
	titleName_->SetText("Sample");
	titleName_->SetScale({ 500,500 });
	titleNamePos_ = { 80.0f,116.0f };
	titleName_->SetCamera(camera_);

	//スタート
	pressStart_ = std::make_unique<Text>();
	pressStart_->Initialize(sceneContext_.object2dCommon,"pressStart");
	pressStart_->SetText("Press : B");
	pressStart_->SetScale({ 500.0f,500.0f });
	pressStartPos_ = {250.0f,600.0f};
	pressStart_->SetCamera(camera_);
}

//更新ww
void TitleScene::Update() {
	//シーンのインタフェースの初期化
	BaseScene::Update();
	
	//シーンの切り替え
	if (sceneContext_.input->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		sceneContext_.sceneManager->ChangeScene("Game");
	}

	titleName_->SetTranslate(titleNamePos_);
	titleName_->SetTextSize(titleNameSize_);
	titleName_->Update();

	pressStart_->SetTranslate(pressStartPos_);
	pressStart_->SetTextSize(pressStartSize_);
	pressStart_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Text("Title");
	ImGui::SeparatorText("TitleName");
	ImGui::DragFloat2("titleNamePos", &titleNamePos_.x, 0.1f);
	ImGui::DragFloat("titleNameSize", &titleNameSize_, 0.1f);
	ImGui::SeparatorText("PressStart");
	ImGui::DragFloat2("pressStartPos", &pressStartPos_.x, 0.1f);
	ImGui::DragFloat("pressStartSize", &pressStartSize_, 0.1f);

	//ImGuiの受付終了
	sceneContext_.imguiManager->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = sceneContext_.cameraManager->FindCamera("titleCamera");
	}
#endif // _DEBUG
}

//描画
void TitleScene::Draw() {
	titleName_->Draw();
	pressStart_->Draw();
}

//終了
void TitleScene::Finalize() {
	//シーンのインターフェースの終了
	BaseScene::Finalize();
}
