#define NOMINMAX
#include "Enemy.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"
#include "func/Math.h"
#include "func/Rendering.h"
#include "WireframeObject3d.h"
#include "func/Collision.h"
#include "func/Physics.h"
#include "Bullet.h"
#include "Score.h"

//コンストラクタ
Enemy::Enemy() {
}

//デストラクタ
Enemy::~Enemy() {
}

//初期化
void Enemy::Initialize(Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName) {
	//オブジェクトの数
	entityGroup_.objectCount = 1;
	//インスタンス数
	//instance_.reserve(100);
	//エンティティの配列の大きさを決める
	entityGroup_.entity.resize(entityGroup_.objectCount);
	//モデル名
	entityGroup_.modelName = modelName;
	//レンダーオブジェクトの初期化
	entityGroup_.renderObject = entityGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	//3dオブジェクトの初期化
	entityGroup_.renderObject.object3d->Initialize(object3dCommon, camera, entityGroup_.objectCount);
	entityGroup_.renderObject.object3d->SetModel(entityGroup_.modelName);

	//ヒットボックス
	entityGroup_.renderObject.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube, entityGroup_.objectCount);

	//ワイヤーフレームの生成
	sphere_ = std::make_unique<WireframeObject3d>();
	sphere_->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kSphere, entityGroup_.objectCount + 1);

	//エンティティ初期化
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//ゲームオブジェクトの初期化
		entityGroup_.entity[i].gameObject.Initialize();
		//ヒットボックスのスケール
		entityGroup_.entity[i].hitBoxScale = Vector3::MakeAllOne();
		//コライダーの状態の初期化
		entityGroup_.entity[i].colliderState.Initialize(entityGroup_.entity[i].hitBoxScale, entityGroup_.entity[i].gameObject, Tag::kEnemy);

		//コライダーの初期化
		entityGroup_.entity[i].collider = entityGroup_.entity[i].collider
			.SetOwner(&entityGroup_.entity[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(false)
			.SetBodyType(BodyType::kDynamic)
			.SetLayer(Layer::kEnemy)
			.SetMaskLayer(ToBits(Layer::kWall) | ToBits(Layer::kGround))
			.SetOnCollision([this, i](ColliderState* other) {this->OnCollision(i, other); })
			.Build();
	}

	//スポーンテーブルのサイズを設定
	enemySpawnTable_.resize(entityGroup_.objectCount);
	enemySpawnTable_[0] = { -8.0f,6.0f,15.0f };
	//enemySpawnTable_[0] = { 12.0f,0.0f,125.0f };
	//enemySpawnTable_[1] = { -10.0f,0.0f,45.0f };
	//enemySpawnTable_[2] = { 12.0f,0.0f,125.0f };

	//敵の位置
	//entityGroup_.entity[0].gameObject.transformData.translate = enemySpawnTable_[0];
	//entityGroup_.entity[0].gameObject.acceleration.y = Physics::kGravity;
	////敵の位置
	//entityGroup_.entity[1].gameObject.transformData.translate = { 5.0f,4.0f,-20.0f };
	//entityGroup_.entity[1].gameObject.acceleration.y = Physics::kGravity;
	////敵の位置
	//entityGroup_.entity[2].gameObject.transformData.translate = { -5.0f,4.0f,-20.0f };
	//entityGroup_.entity[2].gameObject.acceleration.y = Physics::kGravity;

	//敵の状態を生成
	spawn_ = std::make_unique<EnemeyStateSpawn>();
	idol_ = std::make_unique<EnemeyStateIdol>();
	chase_ = std::make_unique<EnemyStateChase>();

	attackArea = std::make_unique<WireframeObject3d>();
	attackArea->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kSphere);
	attackAreaRadius_ = 3.0f;
	attackArea->SetTranslate(0, targetPos_);
	attackArea->Update();
}

//更新
void Enemy::Update() {
	//加速度と速度を適応
	IntegrateMotion();

	int32_t aliveCount = 0;

	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {

		//敵の生成
		if (!entityGroup_.entity[i].gameObject.isAlive) {
			if (spawnTimer_ > 2.0f) {
				ChangeState(spawn_.get());
				spawnTimer_ = 0.0f;
			} else {
				spawnTimer_ += Math::kDeltaTime;
				continue;
			}
		}

		// 生存だけを 0..aliveCount-1 に詰める
		entityGroup_.renderObject.object3d->SetTransformData(
			aliveCount,
			entityGroup_.entity[i].gameObject.transformData
		);

		// ヒットボックスも同じ index にする（デバッグ表示目的なら）
		entityGroup_.renderObject.hitBox->SetTranslate(aliveCount, entityGroup_.entity[i].gameObject.transformData.translate);
		entityGroup_.renderObject.hitBox->SetQuaternion(aliveCount, entityGroup_.entity[i].gameObject.transformData.quaternion);
		entityGroup_.renderObject.hitBox->SetScale(aliveCount, entityGroup_.entity[i].hitBoxScale);

		++aliveCount;
	}

	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		if (entityGroup_.entity[i].gameObject.isAlive) {
			//敵の状態
			currentState_->Exce();

			sphereRadius_ = 7.0f;
			sphere_->SetRadius(i + 1, sphereRadius_);
			sphere_->SetTranslate(i + 1, entityGroup_.entity[i].gameObject.transformData.translate);
			Sphere targetArea = { targetPos_ ,1.0f };
			Sphere myArea = { entityGroup_.entity[i].gameObject.transformData.translate,7.0f };
			if (Collision::IsCollision(targetArea, myArea)) {
				ChangeState(chase_.get());
			} else {
				ChangeState(idol_.get());
			}
		}
	}

	//オブジェクトの更新
	entityGroup_.renderObject.object3d->Update();
	entityGroup_.renderObject.hitBox->Update();

	// 描画に使う数を保存（メンバにして Draw で使う）
	aliveCount_ = aliveCount;

	//プレイヤーの半径
	sphere_->SetRadius(0, 1.0f);
	sphere_->SetTranslate(0, targetPos_);

	sphere_->Update();
}

//デバッグ
void Enemy::Debug() {
#ifdef _DEBUG
	//ImGuiManager::GetInstance()->DragTransform(gameObject_.transformData);
	//ImGui::DragFloat3("hitBox.scale", &hitBoxScale_.x, 0.1f);
	//ImGui::Text("hp:%d", hp_);
	//ImGui::DragFloat3("hp.translate", &hpBarTransform_.translate.x, 0.1f);
	//ImGui::DragFloat3("hp.scale", &hpBarTransform_.scale.x, 0.1f);
	//ImGui::DragFloat("hp.posX", &hpBarPosX_, 0.1f);
	//ImGui::DragFloat3("hpOutLine.translate", &hpOutLineTransform_.translate.x, 0.1f);
	//ImGui::DragFloat3("hpOutLine.scale", &hpOutLineTransform_.scale.x, 0.1f);
	//ImGui::DragFloat3("hitBox.scale", &hitBoxScale_.x, 0.1f);
	//ImGui::Checkbox("collider.isTrigger", &collider_.isTrigger);
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		ImGui::PushID(i);
		ImGui::SeparatorText(("enemy " + std::to_string(i)).c_str());
		ImGui::DragFloat3("hitBox.scale", &entityGroup_.entity[i].hitBoxScale.x, 0.1f);
		ImGuiManager::DragTransform(entityGroup_.entity[i].gameObject.transformData);
		ImGui::DragFloat3("velocity", &entityGroup_.entity[i].gameObject.velocity.x, 0.1f);
		ImGui::DragFloat3("acceleration", &entityGroup_.entity[i].gameObject.acceleration.x, 0.1f);
		ImGui::Checkbox("isAlive", &entityGroup_.entity[i].gameObject.isAlive);
		ImGui::Text("timer%f", spawnTimer_);
		ImGui::PopID();
	}
#endif // _DEBUG
}

//描画
void Enemy::Draw() {
	//敵の描画
	entityGroup_.renderObject.object3d->Draw(aliveCount_);

	//ヒットボックスの描画
	entityGroup_.renderObject.hitBox->Draw(aliveCount_);

	//ワイヤーフレームの描画
	sphere_->Draw(aliveCount_);

	//敵がこのエリアに入ったら動きが変わる
	//attackArea->Draw();
}

//リセット
void Enemy::Reset() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.entity[i].gameObject.transformData.scale = Vector3::MakeAllOne() / 2.0f;
		entityGroup_.entity[i].gameObject.isAlive = true;
	}
}

//衝突したら
void Enemy::OnCollision(int32_t index, ColliderState* other) {
	if (other->tag == Tag::kPlayer) {
		//上から踏まれたら
		if (other->translatePtr->y > entityGroup_.entity[index].gameObject.transformData.translate.y + entityGroup_.entity[index].gameObject.transformData.scale.y) {
			//敵が倒させる
			entityGroup_.entity[index].gameObject.isAlive = false;
			entityGroup_.entity[index].gameObject.velocity = {};
			entityGroup_.entity[index].gameObject.acceleration = {};
			entityGroup_.entity[index].collider.SetIsEnebled(false);
			//スコアの加算
			Score::AddScore(30);
		}
	}
}

//カメラのセッター
void Enemy::SetCamera(Camera* camera) {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.renderObject.object3d->SetCamera(camera);
		entityGroup_.renderObject.hitBox->SetCamera(camera);
	}
	sphere_->SetCamera(camera);
	attackArea->SetCamera(camera);
	//hpBar_->SetCamera(camera);
	//hpOutLine_->SetCamera(camera);
}

//ターゲットの位置
void Enemy::SetTarget(const Vector3& targetPos) {
	targetPos_ = targetPos;
}

//移動速度のセッター
void Enemy::SetMoveSpeed(float moveSpeed) {
	moveSpeed_ = moveSpeed;
}

//エンティティのゲッター
std::vector<Entity>& Enemy::GetEntity() {
	return entityGroup_.entity;
}

//敵の生成
void Enemy::Spawn() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.entity[i].gameObject.isAlive = true;
		entityGroup_.entity[i].collider.SetIsEnebled(true);
		entityGroup_.entity[i].gameObject.transformData.translate = enemySpawnTable_[0];
		entityGroup_.entity[i].gameObject.acceleration.y = Physics::kGravity;
	}
}

//待機
void Enemy::Idol() {
	//for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
	//	entityGroup_.entity[i].gameObject.transformData.rotate.y += kIdolRotSpeed;
	//}
}

//追従
void Enemy::Chase() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//生存してなければ
		if (!entityGroup_.entity[i].gameObject.isAlive) {
			continue;
		}

		//ターゲットの方向を向く
		EnemyToTarget();

		//カメラの角度をもとに回転行列を求める
		Matrix4x4 rotMat = Rendering::MakeRotateMatrix(Rendering::MakeRotateQuaternion(entityGroup_.entity[i].gameObject.transformData.quaternion));
		Vector3 moveDir = { 0.0f,0.0f,1.0f };
		//カメラの向いてる方向を正にする(XとZ軸限定)
		moveDir = Math::TransformNormal(moveDir, rotMat);
		//カメラを移動させる
		entityGroup_.entity[i].gameObject.transformData.translate += moveDir.Normalize() * moveSpeed_;
	}
}

//攻撃
void Enemy::Attack() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//ターゲットの方向を見る
		EnemyToTarget();

		//攻撃する時間を計測
		//if (bulletShotTimer_ < kBulletShotTimerLimit) {
		//	bulletShotTimer_ += Math::kDeltaTime;
		//} else {
		//	bulletShotTimer_ = 0.0f;
		//	//攻撃フラグを立てる
		//	isAttacking_ = true;
		//}

		//攻撃フラグを折る
		isAttacking_ = false;
	}
}


//ターゲットの方向を向く
void Enemy::EnemyToTarget() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//プレイヤーの向きに合わせる
		Vector3 dir = (targetPos_ - entityGroup_.entity[i].gameObject.transformData.translate).Normalize();
		float yaw = std::atan2(dir.x, dir.z);
		entityGroup_.entity[i].gameObject.transformData.quaternion.y = yaw;
	}
}

//敵の振る舞い
void Enemy::Behavior() {
	////当たり判定
	//if (Collision::IsCollision(sphere_->GetSphere(0), attackArea->GetSphere(0))) {
	//	//敵の状態を設定
	//	enemyState_ = std::make_unique<EnemeyStateAttack>();

	//	//ワイヤーフレームの色を変更
	//	sphere_->SetColor(Vector4::MakeRedColor());
	//	attackArea->SetColor(Vector4::MakeRedColor());
	//} else {

	//	//攻撃タイマーの加算
	//	if (attackTimer_ < kAttackTimerLimit) {
	//		attackTimer_ += Math::kDeltaTime;
	//	}

	//	//攻撃タイマーが攻撃タイマーのリミット以上になったら
	//	if (attackTimer_ >= kAttackTimerLimit) {
	//		//敵がプレイヤーを追う
	//		enemyState_ = std::make_unique<EnemyStateChase>();
	//		//攻撃タイマーをリセット
	//		attackTimer_ = 0.0f;
	//	}

	//	//ワイヤーフレームの色を変更
	//	sphere_->SetColor(Vector4::MakeBlackColor());
	//	attackArea->SetColor(Vector4::MakeBlackColor());
	//}

	////敵の状態
	//enemyState_->SetEnemy(this);
	//enemyState_->Exce();
}

//速度と加速度を位置に適応
void Enemy::IntegrateMotion() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//生存してなければ
		if (!entityGroup_.entity[i].gameObject.isAlive) {
			continue;
		}
		//加速度を適応
		entityGroup_.entity[i].gameObject.velocity += entityGroup_.entity[i].gameObject.acceleration * Math::kDeltaTime;
		//速度を適応
		entityGroup_.entity[i].gameObject.transformData.translate += entityGroup_.entity[i].gameObject.velocity * Math::kDeltaTime;
	}
}

//ステートの切り替え
void Enemy::ChangeState(IEnemyState* next) {
	currentState_ = next;
	currentState_->SetEnemy(this);
	//敵の状態
	currentState_->Exce();
}
