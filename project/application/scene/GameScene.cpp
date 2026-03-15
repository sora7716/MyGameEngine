#define NOMINMAX
#include "GameScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "CameraManager.h"
#include "WireframeObject3d.h"
#include "Core.h"
#include "field/Ground.h"
#include "character/player/Player.h"
#include "algorithm/ColliderManager.h"


//コンストラクタ
GameScene::GameScene() {
}

//デストラクタ
GameScene::~GameScene() {
}

//初期化
void GameScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	IScene::Initialize(sceneContext);
	//カメラの設定
	camera_ = sceneContext_.cameraManager->FindCamera("gameCamera");

	ground_ = std::make_unique<Ground>();
	ground_->Initialize(sceneContext_.object3dCommon, camera_);

	player_ = std::make_unique<Player>();
	player_->Initialize(sceneContext_.object3dCommon, camera_);

	for (Entity& entity : ground_->GetEntity()) {
		colliderManager_->AddCollider(&entity.collider);
	}

	for (Entity& entity : player_->GetEntity()) {
		colliderManager_->AddCollider(&entity.collider);
	}
}

//更新
void GameScene::Update() {

	//シーンのインタフェースの初期化
	IScene::Update();

	ground_->Update();

	player_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//地面
	ImGui::Begin("ground");
	ground_->Debug();
	ImGui::End();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();
	//ImGuiの受付終了
	sceneContext_.imguiManager->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	//カメラの切り替え
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		//camera_ = gameCamera_->GetCamera();
		camera_ = sceneContext_.cameraManager->FindCamera("gameCamera");
	}
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
	ground_->Draw();

	player_->Draw();
}

//終了
void GameScene::Finalize() {
	//シーンのインターフェース
	IScene::Finalize();
}
