#define NOMINMAX
#include "GameScene.h"
#include "Input.h"
#include "Object3dCommon.h"
#include "GlobalVariables.h"
#include "func/Collision.h"
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
#include "ColliderManager.h"
#include "func/Math.h"
#include "func/Physics.h"

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

	//追従カメラ
	gameCamera_ = std::make_unique<GameCamera>();
	gameCamera_->Initialize(sceneContext_.input,camera_);

	//プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(sceneContext_.input, sceneContext_.spriteCommon, sceneContext_.object3dCommon, gameCamera_->GetCamera(), "player");
	player_->SetPosition({ 0.0f,0.0f,-25.0f });

	//enemy_ = std::make_unique<Enemy>();
	//enemy_->Initialize(sceneContext_.object3dCommon,camera_,"enemy");
	//enemy_->SetTranslate({ 22.0f,0.0f,25.0f });

	//フィールド
	field_ = std::make_unique<Field>();
	field_->Initialize(sceneContext_.object3dCommon, camera_);

	//敵の実装
	//enemyManager_ = EnemyManager::GetInstance();
	//enemyManager_->Initialize(sceneContext_.object3dCommon, camera_);

	//スコア
	score_ = std::make_unique<Score>();
	score_->Initialize(sceneContext_.object2dCommon);

	//衝突判定
	//プレイヤー
	colliderManager_->AddCollider(&player_->GetCollider());

	//壁
	for (int32_t i = 0; i < static_cast<int32_t>(field_->GetWallDescs().size()); i++) {
		colliderManager_->AddCollider(&field_->GetWallDescs()[i].collider);
	}

	//地面
	for (int32_t i = 0; i < static_cast<int32_t>(field_->GetGroundDesc().size()); i++) {
		colliderManager_->AddCollider(&field_->GetGroundDesc()[i].collider);
	}
	//colliderManager_->AddCollider(&enemy_->GetCollider());
}

//更新
void GameScene::Update() {
	//追従カメラ
	gameCamera_->Update();
	gameCamera_->SetTragetPos(player_->GetTransformData().translate);

	//カメラの設定
	player_->SetCamera(camera_);
	field_->SetCamera(camera_);
	//enemyManager_->SetCamera(camera_);
	//enemy_->SetCamera(camera_);

	Vector3 prePlayerPos = player_->GetTransformData().translate;

	//プレイヤー
	player_->Update();
	Vector3 playerPos = player_->GetTransformData().translate;
	sceneContext_.object3dCommon->SetPointLightPos({ playerPos.x,playerPos.y + 2.0f,playerPos.z });

	//敵
	//enemyManager_->Update(player_->GetWorldPos());

	//enemy_->Update();

	//フィールド
	field_->Update();

	//スコア
	score_->Update();

	////衝突判定
	////敵とプレイヤーの弾
	//for (Enemy* enemy : enemyManager_->GetEnemies()) {
	//	if (!enemy->IsAlive()) {
	//		continue;//敵が生存していなかったら
	//	}
	//	for (uint32_t i = 0; i < player_->GetBullet()->GetBulletData().size(); i++) {
	//		if (player_->GetBullet()->GetBulletData()[i].gameObject.isAlive) {
	//			if (Collision::IsCollision(enemy->GetOBB(), player_->GetBullet()->GetBulletData()[i].gameObject.hitBox->GetOBB())) {
	//				player_->GetBullet()->OnCollision(i);
	//				enemy->OnCollision();
	//				break;
	//			}
	//		}
	//	}
	//}

	////プレイヤーと敵の弾
	//for (Enemy* enemy : enemyManager_->GetEnemies()) {
	//	for (uint32_t i = 0; i < enemy->GetBullet()->GetBulletData().size(); i++) {
	//		if (enemy->GetBullet()->GetBulletData()[i].gameObject.isAlive) {
	//			if (Collision::IsCollision(player_->GetOBB(), enemy->GetBullet()->GetBulletData()[i].gameObject.hitBox->GetOBB())) {
	//				enemy->GetBullet()->OnCollision(i);
	//				player_->OnCollision();
	//				break;
	//			}
	//		}
	//	}
	//}

	////プレイヤーが死んだら
	//if (!player_->IsAlive()) {
	//	//sceneContext_.sceneManager->ChangeScene("Result");
	//}

	//終了フラグが経ったら
	if (player_->IsGoal()) {
		sceneContext_.sceneManager->ChangeScene("Result");
	}

	//シーンのインタフェースの初期化
	IScene::Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	//フィールド
	ImGui::Begin("field");
	field_->Debug();
	ImGui::End();

	//グローバル変数の更新
	//GlobalVariables::GetInstance()->Update();

	//プレイヤー
	ImGui::Begin("player");
	player_->Debug();
	ImGui::End();

	//ImGui::Begin("enemy");
	//enemy_->Debug();
	//ImGui::End();

	//ゲームカメラ
	ImGui::Begin("gameCamera");
	gameCamera_->Debug();
	ImGui::End();

	//敵
	//ImGui::Begin("enemy");
	//enemyManager_->Debug();
	//ImGui::End();

	//スコア
	//ImGui::Text("score:%d", Score::score);

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
		camera_ = gameCamera_->GetCamera();
	}
#endif // _DEBUG

}

//描画
void GameScene::Draw() {
	//プレイヤー
	player_->Draw();

	//enemy_->Draw();

	//マップチップ
	field_->Draw();

	//敵
	//enemyManager_->Draw();

	//スコア
	score_->Draw();
}

//終了
void GameScene::Finalize() {
	//敵の解放
	EnemyManager::GetInstance()->Finalize();
	//シーンのインターフェース
	IScene::Finalize();
}
