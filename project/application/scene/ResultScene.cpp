#include "ResultScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include <sstream>
#include <iomanip>
#include "Text.h"
#include "field/StageTimer.h"

//コンストラクタ
ResultScene::ResultScene() {}

//デストラクタ
ResultScene::~ResultScene() {}

//初期化
void ResultScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	BaseScene::Initialize(sceneContext);
	camera_ = *sceneContext_.cameraManager->FindCamera("ResultCamera");

	//スコア
	drawScore_ = std::make_unique<Text>();
	drawScore_->Initialize(sceneContext_.object2dCommon, "drawScore");
	//スコアの文字列を作成
	std::ostringstream scoreText;
	scoreText << "SCORE : " << std::setw(3) << std::setfill('0') << 100.0f - stageTimer_->timer_;
	//タイマーをリセット
	stageTimer_->timer_ = 100.0f;
	drawScore_->SetText(scoreText.str());

	//PressReturn
	pressReturn_ = std::make_unique<Text>();
	pressReturn_->Initialize(sceneContext_.object2dCommon, "pressReturn");
	pressReturn_->SetText("Press : B");
}

//更新ww
void ResultScene::Update() {
	//シーンのインタフェースの初期化
	BaseScene::Update();

	if (sceneContext_.input->TriggerKey(DIK_SPACE)) {
		sceneContext_.sceneManager->ChangeScene("Title");
	} else if (sceneContext_.input->TriggerXboxPad(xBoxPadNumber_, XboxInput::kB)) {
		sceneContext_.sceneManager->ChangeScene("Title");
	}

	drawScore_->SetTranslate(scorePos_);
	drawScore_->SetScale(scoreScale_);
	drawScore_->SetTextSize(scoreTextSize_);
	drawScore_->Update();

	pressReturn_->SetTranslate(pressReturnPos_);
	pressReturn_->SetScale(scoreScale_);
	pressReturn_->SetTextSize(pressReturnSize_);
	pressReturn_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::SeparatorText("socre");
	ImGui::PushID(0);
	ImGui::DragFloat2("scale", &scoreScale_.x, 0.1f);
	ImGui::DragFloat2("position", &scorePos_.x, 0.1f);
	ImGui::DragFloat("textSize", &scoreTextSize_, 0.1f);
	ImGui::PopID();
	ImGui::SeparatorText("pressReturn");
	ImGui::PushID(1);
	ImGui::DragFloat2("position", &pressReturnPos_.x, 0.1f);
	ImGui::DragFloat("textSize", &pressReturnSize_, 0.1f);
	ImGui::PopID();
	ImGui::End();
	ImGui::Text("Result");
	//ImGuiの受付終了
	sceneContext_.imguiManager->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = *debugCamera_->GetCamera();
	} else {
		camera_ = *sceneContext_.cameraManager->FindCamera("ResultCamera");
	}
#endif // _DEBUG


}

//描画
void ResultScene::Draw() {
	drawScore_->Draw();
	pressReturn_->Draw();
}

//終了
void ResultScene::Finalize() {
	//シーンのインターフェースの終了
	BaseScene::Finalize();
}
