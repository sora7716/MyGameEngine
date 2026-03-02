#define NOMINMAX
#include "EnemyManager.h"
#include "engine/math/func/Math.h"
#include "engine/debug/ImGuiManager.h"
#include <cassert>
//インスタンスのゲッター
EnemyManager* EnemyManager::GetInstance() {
	if (instance == nullptr) {
		instance = new EnemyManager();
	}
	return instance;

}

//初期化
void EnemyManager::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//3Dオブジェクトの共通部分を記録
	object3dCommon_ = object3dCommon;
	//カメラを記録
	camera_ = camera;
	//ランダムエンジンの初期化
	//乱数エンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());

	//敵の数を設定
	enemies_.resize(kMaxEnemy);
	for (Enemy*& enemy : enemies_) {
		enemy = new Enemy();
		enemy->Initialize(object3dCommon, camera, "enemy");
	}

	//最初のスポーンする敵を設定
	for (int32_t i = 0; i < (kFirstSpawnEnemyCount > kMaxEnemy ? kMaxEnemy : kFirstSpawnEnemyCount); i++) {
		SetSpawnParams(enemies_[i]);
	}
}

//更新
void EnemyManager::Update(const Vector3& target) {
	//敵のスポーン
	Spawn();

	//ターゲットを中心としてエリアを展開
	SetSpawnAreaCenter(target);

	//敵の行動処理
	for (Enemy* enemy : enemies_) {
		//if (enemy->IsAlive()) {
		//	enemy->SetCamera(camera_);
		//	enemy->SetTarget(target);
		//	enemy->Update();
		//}
	}
}

//デバッグ
void EnemyManager::Debug() {
#ifdef _DEBUG
	ImGui::Text("interval:%f", spawnInterval_);
	ImGui::Text("timer:%f", spawnTimer_);
#endif // _DEBUG

}

//描画
void EnemyManager::Draw() {
	for (Enemy* enemy : enemies_) {
		//if (enemy->IsAlive()) {
		//	enemy->Draw();
		//}
	}
}

//カメラのセッター
void EnemyManager::SetCamera(Camera* camera) {
	camera_ = camera;
}

//スポーンエリアの中心のセッター
void EnemyManager::SetSpawnAreaCenter(const Vector3& spawnAreaCenter) {
	spawnAreaCenter_ = spawnAreaCenter;
}

//敵のリストを取得
const std::vector<Enemy*>& EnemyManager::GetEnemies() const {
	return enemies_;
}

//終了
void EnemyManager::Finalize() {
	//敵の解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	//インスタンスの解放
	delete instance;
	instance = nullptr;
}

//敵をスポーンのパラメータを設定
void EnemyManager::SetSpawnParams(Enemy* enemy) {
	//敵を値をリセット
	enemy->Reset();
	//位置の値をemitRange_の範囲でランダムに設定
	Vector3 spawnAreaMin = { -(spawnAreaCenter_.x + spawnAreaRadius_),0.0f,-(spawnAreaCenter_.z + spawnAreaRadius_) };
	Vector3 spawnAreaMax = { spawnAreaCenter_.x + spawnAreaRadius_,0.0f,spawnAreaCenter_.z + spawnAreaRadius_ };
	std::uniform_real_distribution<float>distributionPositionX(spawnAreaMin.x, spawnAreaMax.x);
	std::uniform_real_distribution<float>distributionPositionZ(spawnAreaMin.z, spawnAreaMax.z);
	//enemy->SetTranslate({ distributionPositionX(randomEngine_),0.0f,distributionPositionZ(randomEngine_) });

	//移動速度をランダム
	std::uniform_real_distribution<float>distributionMoveSpeed(0.1f, 0.15f);
	enemy->SetMoveSpeed(distributionMoveSpeed(randomEngine_));

	//弾の発射速度をランダム
	std::uniform_real_distribution<float>distributionBulletShotSpeed(-64.0f, -8.0f);
	//enemy->SetBulletShotSpeed(distributionBulletShotSpeed(randomEngine_));
}

//敵をスポーン
void EnemyManager::Spawn() {
	if (spawnTimer_ < spawnInterval_) {
		spawnTimer_ += Math::kDeltaTime;
	} else {
		//スポーンタイマーをリセット
		spawnTimer_ = 0;
		//敵のスポーンのパラメータを設定
		for (Enemy* enemy : enemies_) {
			//if (!enemy->IsAlive()) {
			//	SetSpawnParams(enemy);
			//	break;
			//}
		}

		//spawnIntervalを少し減らす
		 // ゆっくり縮める：1回のスポーンで減る量を定数化
		float kIntervalDecreasePerSpawn = 0.15f; // ここを小さくするとゆっくり
		float kMinSpawnInterval = 1.0f;

		spawnInterval_ = std::max(kMinSpawnInterval, spawnInterval_ - kIntervalDecreasePerSpawn);
	}
}