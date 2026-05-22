#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "ImGuiManager.h"
#include "Box.h"

//コンストラクタ
TestPlayScene::TestPlayScene() {};

//デストラクタ
TestPlayScene::~TestPlayScene() {};

//初期化
void TestPlayScene::Initialize(const SceneContext& sceneContext) {
	//ベースシーンの初期化
	BaseScene::Initialize(sceneContext);
	camera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");

	box_ = std::make_unique<Box>();
	box_->Initialize(sceneContext_.directXBase, sceneContext_.textureManager, &camera_);
	//box_->SetModel("cube");
}

//更新
void TestPlayScene::Update() {
	//ベースシーンの更新
	BaseScene::Update();

	box_->Update();
}

//デバッグ
void TestPlayScene::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("box");
	box_->Debug();
	ImGui::End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = *debugCamera_->GetCamera();
	} else {
		camera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");
	}
#endif // _DEBUG
}

//描画
void TestPlayScene::Draw() {
	box_->Draw();
}

//終了
void TestPlayScene::Finalize() {
	//ベースシーンのの終了
	BaseScene::Finalize();
}
