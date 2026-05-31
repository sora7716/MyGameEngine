#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "ImGuiManager.h"
#include "Box.h"
#include "Object3d.h"

//コンストラクタ
TestPlayScene::TestPlayScene() {};

//デストラクタ
TestPlayScene::~TestPlayScene() {};

//初期化
void TestPlayScene::Initialize(const SceneContext& sceneContext) {
	//ベースシーンの初期化
	BaseScene::Initialize(sceneContext);
	camera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");

	//box_ = std::make_unique<Box>();
	//box_->Initialize(sceneContext_.directXBase, sceneContext_.textureManager, &camera_);
	//box_->SetModel("cube");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneContext_.object3dCommon,&camera_);
	object3d_->SetModel("multiMesh");
}

//更新
void TestPlayScene::Update() {
	//ベースシーンの更新
	BaseScene::Update();

	object3d_->Update();

	//box_->Update();
}

//デバッグ
void TestPlayScene::Debug() {
#ifdef USE_IMGUI
	//ImGui::Begin("box");
	//box_->Debug();
	//ImGui::End();

	ImGui::Begin("object3d");
	GameObject gameObject = object3d_->GetGameObject(0);
	ImGuiManager::DebugGameObject(gameObject);
	object3d_->SetGameObject(0,gameObject);
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
	//box_->Draw();

	object3d_->Draw();
}

//終了
void TestPlayScene::Finalize() {
	//ベースシーンのの終了
	BaseScene::Finalize();
}
