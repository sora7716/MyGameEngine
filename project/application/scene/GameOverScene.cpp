#include "GameOverScene.h"
#include "engine/input/Input.h"
#include "engine/camera/CameraManager.h"
#include "engine/scene/SceneManager.h"
#include "engine/2d/Text.h"
#include "engine/base/Core.h"

//コンストラクタ
GameOverScene::GameOverScene() {};

//デストラクタ
GameOverScene::~GameOverScene() {};

//初期化
void GameOverScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	IScene::Initialize(sceneContext);
	camera_ = sceneContext_.cameraManager->FindCamera("titleCamera");

	//タイトル名
	gameOver_ = std::make_unique<Text>();
	gameOver_->Initialize(sceneContext_.object2dCommon, "gameOver");
	gameOver_->SetText("GameOver");
	gameOver_->SetScale({ 500,500 });
	gameOverPos_ = { 51.0f,250.0f };
	gameOver_->SetCamera(camera_);

	//スタート
	pressReturn_ = std::make_unique<Text>();
	pressReturn_->Initialize(sceneContext_.object2dCommon, "pressRestart(GameOver)");
	pressReturn_->SetText("Press : B");
	pressReturn_->SetScale({ 500.0f,500.0f });
	pressStartPos_ = { 250.0f,600.0f };
	pressReturn_->SetCamera(camera_);
}

//更新ww
void GameOverScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	//シーンの切り替え
	if (sceneContext_.input->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		sceneContext_.sceneManager->ChangeScene("Game");
	}

	gameOver_->SetTranslate(gameOverPos_);
	gameOver_->SetTextSize(gameOverSize_);
	gameOver_->Update();

	pressReturn_->SetTranslate(pressStartPos_);
	pressReturn_->SetTextSize(pressStartSize_);
	pressReturn_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	ImGui::Text("GameOver");
	ImGui::SeparatorText("GameOver");
	ImGui::DragFloat2("GameOverPos", &gameOverPos_.x, 0.1f);
	ImGui::DragFloat("GameOverSize", &gameOverSize_, 0.1f);
	ImGui::SeparatorText("PressReturn");
	ImGui::DragFloat2("PressReturnPos", &pressStartPos_.x, 0.1f);
	ImGui::DragFloat("PressReturnSize", &pressStartSize_, 0.1f);

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
void GameOverScene::Draw() {
	gameOver_->Draw();
	pressReturn_->Draw();
}

//終了
void GameOverScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
