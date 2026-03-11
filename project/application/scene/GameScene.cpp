#define NOMINMAX
#include "GameScene.h"
#include "Input.h"
#include "Object3dCommon.h"
#include "algorithm/Collision.h"
#include "SceneManager.h"
#include "CameraManager.h"
#include "WireframeObject3d.h"
#include "Core.h"
#include "Bullet.h"
#include "Player.h"
#include "GameCamera.h"
#include "Field.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "Score.h"
//#include "ColliderManager.h"
#include "algorithm/Math.h"
#include "algorithm/Physics.h"
#include "StageTimer.h"
#include "Item.h"

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

	////追従カメラ
	//gameCamera_ = std::make_unique<GameCamera>();
	//gameCamera_->Initialize(sceneContext_.input, camera_);

	////プレイヤー
	//player_ = std::make_unique<Player>();
	//player_->Initialize(sceneContext_.input, sceneContext_.spriteCommon, sceneContext_.object3dCommon, gameCamera_->GetCamera(), "player");
	//player_->SetPosition({ 0.0f,0.0f,-25.0f });

	//enemy_ = std::make_unique<Enemy>();
	//enemy_->Initialize(sceneContext_.object3dCommon, camera_, "enemy");
	////enemy_->SetTranslate({ 0.0f,1.0f,-20.0f });

	////フィールド
	//field_ = std::make_unique<Field>();
	//field_->Initialize(sceneContext_.object3dCommon, camera_);

	////敵の実装
	////enemyManager_ = EnemyManager::GetInstance();
	////enemyManager_->Initialize(sceneContext_.object3dCommon, camera_);

	////スコア
	//score_ = std::make_unique<Score>();
	//score_->Initialize(sceneContext_.object2dCommon);

	////ステージタイマー
	//stageTimer_ = std::make_unique<StageTimer>();
	//stageTimer_->Initialize(sceneContext_.object2dCommon);

	//item_ = std::make_unique<Item>();
	//item_->Initialize(sceneContext_.object3dCommon, camera_, "plane");

	////衝突判定
	////プレイヤー
	//colliderManager_->AddCollider(&player_->GetCollider());

	////壁
	//for (int32_t i = 0; i < static_cast<int32_t>(field_->GetWallDescs().size()); i++) {
	//	colliderManager_->AddCollider(&field_->GetWallDescs()[i].collider);
	//}

	////地面
	//for (int32_t i = 0; i < static_cast<int32_t>(field_->GetGroundDesc().size()); i++) {
	//	colliderManager_->AddCollider(&field_->GetGroundDesc()[i].collider);
	//}

	////敵
	//for (int32_t i = 0; i < static_cast<int32_t>(enemy_->GetEntity().size()); i++) {
	//	colliderManager_->AddCollider(&enemy_->GetEntity()[i].collider);
	//}

	////アイテム
	//for (int32_t i = 0; i < static_cast<int32_t>(item_->GetEntity().size()); i++) {
	//	colliderManager_->AddCollider(&item_->GetEntity()[i].collider);
	//}
}

//更新
void GameScene::Update() {
	////追従カメラ
	//gameCamera_->SetTragetPos(player_->GetTransformData().translate);
	//gameCamera_->Update();

	////カメラの設定
	//player_->SetCamera(camera_);
	//field_->SetCamera(camera_);
	//enemy_->SetCamera(camera_);
	//item_->SetCamera(camera_);
	////enemyManager_->SetCamera(camera_);

	//Vector3 prePlayerPos = player_->GetTransformData().translate;

	////プレイヤー
	//player_->Update();
	//Vector3 playerPos = player_->GetTransformData().translate;
	//sceneContext_.object3dCommon->SetPointLightPos({ playerPos.x,playerPos.y + 2.0f,playerPos.z });

	////敵
	////enemyManager_->Update(player_->GetWorldPos());

	//enemy_->SetTarget(player_->GetWorldPos());
	//enemy_->Update();

	////フィールド
	//field_->Update();

	////スコア
	//score_->Update();

	////ステージタイマー
	//stageTimer_->Update();

	//item_->Update();

	////シーンの切り替え
	//if (player_->IsGoal()) {
	//	//プレイヤーがゴールしたら
	//	sceneContext_.sceneManager->ChangeScene("Result");
	//} else if (!player_->IsAlive()) {
	//	//プレイヤーが死んだら
	//	sceneContext_.sceneManager->ChangeScene("Result");
	//} else if (stageTimer_->IsTimeUp()) {
	//	//時間切れを起こしたら
	//	sceneContext_.sceneManager->ChangeScene("Result");
	//}

	//シーンのインタフェースの初期化
	IScene::Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	////フィールド
	//ImGui::Begin("field");
	//field_->Debug();
	//ImGui::End();

	////グローバル変数の更新
	////GlobalVariables::GetInstance()->Update();

	////プレイヤー
	//ImGui::Begin("player");
	//player_->Debug();
	//ImGui::End();

	////ImGui::Begin("enemy");
	////enemy_->Debug();
	////ImGui::End();

	////ゲームカメラ
	//ImGui::Begin("gameCamera");
	//gameCamera_->Debug();
	//ImGui::End();

	////敵
	//ImGui::Begin("enemy");
	//enemy_->Debug();
	//ImGui::End();

	////ステージタイマー
	//ImGui::Begin("stageTimer");
	//stageTimer_->Debug();
	//ImGui::End();

	////スコア
	//ImGui::Begin("score");
	//score_->Debug();
	//ImGui::End();

	////アイテム
	//ImGui::Begin("Item");
	//item_->Debug();
	//ImGui::End();

	//Object3dCommon
	//sceneContext_.object3dCommon->Debug();

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
	////プレイヤー
	//player_->Draw();

	////敵
	//enemy_->Draw();

	////アイテム
	//item_->Draw();

	////マップチップ
	//field_->Draw();

	////敵
	////enemyManager_->Draw();

	////スコア
	//score_->Draw();
	//
	////ステージタイマー
	//stageTimer_->Draw();
}

//終了
void GameScene::Finalize() {
	//敵の解放
	//EnemyManager::GetInstance()->Finalize();
	//シーンのインターフェース
	IScene::Finalize();
}
