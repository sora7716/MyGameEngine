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

	//エンティティ初期化
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//ゲームオブジェクトの初期化
		entityGroup_.entity[i].gameObject.Initialize();
		//ヒットボックスのスケール
		entityGroup_.entity[i].hitBoxScale = Vector3::MakeAllOne();
		//コライダーの状態の初期化
		entityGroup_.entity[i].colliderState.Initialize(entityGroup_.entity[i].hitBoxScale, entityGroup_.entity[i].gameObject, entityGroup_.renderObject.object3d->GetWorldMatrix(i),Tag::kEnemy);

		//コライダーの初期化
		entityGroup_.entity[i].collider = entityGroup_.entity[i].collider
			.SetOwner(&entityGroup_.entity[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(true)
			.SetBodyType(BodyType::kDynamic)
			.SetLayer(Layer::kEnemy)
			.SetMaskLayer(ToBits(Layer::kWall) | ToBits(Layer::kGround) | ToBits(Layer::kEnemy))
			.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
			.Build();
	}

	//敵の位置
	entityGroup_.entity[0].gameObject.transformData.translate = { 0.0f,4.0f,-20.0f };
	entityGroup_.entity[0].gameObject.acceleration.y = Physics::kGravity;

	//弾の生成と初期化
	bullet_ = std::make_unique<Bullet>();
	bullet_->Initialize(object3dCommon, camera);
	bullet_->SetAliveRange(kAliveAreaSize);
	bullet_->SetSpeed(bulletShotSpeed_);
	bullet_->SetSize({ kBulletSize,kBulletSize,kBulletSize });
	bullet_->SetMaxBulletCount(kBulletCount);

	//敵の状態を生成
	enemyState_ = std::make_unique<EnemyStateChase>();
	enemyState_->SetEnemy(this);

	//ワイヤーフレームの生成
	sphere_ = std::make_unique<WireframeObject3d>();
	sphere_->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kSphere);
	sphereRadius_ = 2.0f;

	attackArea = std::make_unique<WireframeObject3d>();
	attackArea->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kSphere);
	attackAreaRadius_ = 3.0f;
	attackArea->SetTranslate(0, targetPos_);
	attackArea->Update();

	//HP
	//hpBar_ = std::make_unique<Object3d>();
	//hpBar_->Initialize(object3dCommon, camera, 1, Transform3dMode::kBilboard);
	//hpBar_->SetModel("hpBar");
	//hpBar_->SetTexture("playerHpBar.png");
	//hpBarTransform_.scale = { hpBarWidth_,0.2f,1.0f };
	//Material hpMaterial;
	//hpMaterial.color = Vector4::MakeRedColor();
	//hpMaterial.enableLighting = false;
	//hpBar_->GetModel()->SetMaterial(hpMaterial);

	//Material hpOutLineMaterial;
	//hpOutLineMaterial.color = Vector4::MakeWhiteColor();
	//hpOutLineMaterial.enableLighting = false;

	//hpOutLine_ = std::make_unique<Object3d>();
	//hpOutLine_->Initialize(object3dCommon, camera, 1, Transform3dMode::kBilboard);
	//hpOutLine_->SetModel("hpOutLine");
	//hpOutLine_->SetTexture("playerHpOutLine.png");
	//hpOutLine_->GetModel()->SetMaterial(hpOutLineMaterial);
	//hpOutLineTransform_.scale = { hpBarWidth_,0.2f,1.0f };
}

//更新
void Enemy::Update() {
	//オブジェクト3Dの更新
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.renderObject.object3d->SetTransformData(i, entityGroup_.entity[i].gameObject.transformData);
		entityGroup_.renderObject.object3d->Update();
		//ワイヤーフレームの更新
		// //球
		sphere_->SetRadius(i, sphereRadius_);
		sphere_->SetTranslate(i, entityGroup_.entity[i].gameObject.transformData.translate);
		sphere_->Update();

		//ヒットボックス
		entityGroup_.renderObject.hitBox->SetTranslate(0, entityGroup_.renderObject.object3d->GetWorldPos(i));
		entityGroup_.renderObject.hitBox->SetRotate(0, entityGroup_.entity[i].gameObject.transformData.rotate);
		entityGroup_.renderObject.hitBox->SetScale(0, entityGroup_.entity[i].hitBoxScale);
		entityGroup_.renderObject.hitBox->Update();
	}

	//速度と加速度を適応
	IntegrateMotion();

	//このエリアに敵が入ったら動きが変わる
	attackArea->SetRadius(0, attackAreaRadius_);
	attackArea->SetTranslate(0, targetPos_);
	attackArea->Update();

	//振る舞い
	//Behavior();

	//弾の更新
	bullet_->Update();

	//HP
	//Vector3 worldPos = renderObject_.object3d->GetWorldPos(0);
	//hpOutLineTransform_.translate = { worldPos.x,worldPos.y + 2.0f,worldPos.z };
	//hpOutLine_->SetTransformData(0, hpOutLineTransform_);
	//hpOutLine_->Update();
	//hpBarTransform_.translate = { worldPos.x + hpBarPosX_,worldPos.y + 2.0f,worldPos.z };
	//hpBar_->SetTransformData(0, hpBarTransform_);
	//hpBar_->Update();
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
		ImGui::PopID();
	}
#endif // _DEBUG
}

//描画
void Enemy::Draw() {
	//敵の描画
	entityGroup_.renderObject.object3d->Draw();

	//ヒットボックスの描画
	entityGroup_.renderObject.hitBox->Draw();

	//弾の描画
	bullet_->Draw();

	//ワイヤーフレームの描画
	//sphere_->Draw();

	//敵がこのエリアに入ったら動きが変わる
	//attackArea->Draw();

	//HP
	//hpBar_->Draw();
	//hpOutLine_->Draw();
}

//リセット
void Enemy::Reset() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.entity[i].gameObject.transformData.scale = Vector3::MakeAllOne() / 2.0f;
		entityGroup_.entity[i].gameObject.isAlive = true;
	}
	//HP
	//hp_ = kMaxHpCout;
	//hpBarTransform_.scale = { hpBarWidth_,0.2f,1.0f };
	//hpBarTransform_.translate = {};
	//hpOutLineTransform_.scale = { hpBarWidth_,0.2f,1.0f };
	//hpOutLineTransform_.translate = {};
	//hpBarPosX_ = 0.0f;
}

//衝突したら
void Enemy::OnCollision(ColliderState* other) {
	if (other->tag == Tag::kPlayer) {
		//hp_--;
		////描画のHPにも適応
		//hpBarTransform_.scale.x -= hpBarWidth_ / kMaxHpCout;
		//hpBarPosX_ -= hpBarWidth_ / kMaxHpCout;
		//if (hp_ <= 0) {
		//	gameObject_.isAlive = false;
		//	//スコアを加算
		//	Score::AddScore(30);
		//}
	}
}

//待機
void Enemy::Idol() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.entity[i].gameObject.transformData.rotate.y += kIdolRotSpeed;
	}
}

//追従
void Enemy::Chase() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//ターゲットの方向を向く
		EnemyToTarget();

		//カメラの角度をもとに回転行列を求める
		Matrix4x4 rotMat = Rendering::MakeRotateXYZMatrix(entityGroup_.entity[i].gameObject.transformData.rotate);
		Vector3 moveDir = { 0.0f,0.0f,-1.0f };
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
		if (bulletShotTimer_ < kBulletShotTimerLimit) {
			bulletShotTimer_ += Math::kDeltaTime;
		} else {
			bulletShotTimer_ = 0.0f;
			//攻撃フラグを立てる
			isAttacking_ = true;
		}

		//弾の発射
		bullet_->SetShootingPosition(entityGroup_.renderObject.object3d->GetWorldPos(i));
		bullet_->SetSourceWorldMatrix(entityGroup_.renderObject.object3d->GetWorldMatrix(i));
		bullet_->Fire(isAttacking_);

		//攻撃フラグを折る
		isAttacking_ = false;
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

//弾の発射速度のセッター
void Enemy::SetBulletShotSpeed(float bulletShotSpeed) {
	bulletShotSpeed_ = bulletShotSpeed;
}

//弾のゲッター
Bullet* Enemy::GetBullet() const {
	return bullet_.get();
}

//エンティティのゲッター
std::vector<Entity>& Enemy::GetEntity() {
	return entityGroup_.entity;
}

//ターゲットの方向を向く
void Enemy::EnemyToTarget() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//プレイヤーの向きに合わせる
		Vector3 dir = (entityGroup_.renderObject.object3d->GetWorldPos(i) - targetPos_).Normalize();
		float yaw = std::atan2(dir.x, dir.z);
		entityGroup_.entity[i].gameObject.transformData.rotate.y = yaw;
	}
}

//敵の振る舞い
void Enemy::Behavior() {
	//当たり判定
	if (Collision::IsCollision(sphere_->GetSphere(0), attackArea->GetSphere(0))) {
		//敵の状態を設定
		enemyState_ = std::make_unique<EnemeyStateAttack>();

		//ワイヤーフレームの色を変更
		sphere_->SetColor(Vector4::MakeRedColor());
		attackArea->SetColor(Vector4::MakeRedColor());
	} else {

		//攻撃タイマーの加算
		if (attackTimer_ < kAttackTimerLimit) {
			attackTimer_ += Math::kDeltaTime;
		}

		//攻撃タイマーが攻撃タイマーのリミット以上になったら
		if (attackTimer_ >= kAttackTimerLimit) {
			//敵がプレイヤーを追う
			enemyState_ = std::make_unique<EnemyStateChase>();
			//攻撃タイマーをリセット
			attackTimer_ = 0.0f;
		}

		//ワイヤーフレームの色を変更
		sphere_->SetColor(Vector4::MakeBlackColor());
		attackArea->SetColor(Vector4::MakeBlackColor());
	}

	//敵の状態
	enemyState_->SetEnemy(this);
	enemyState_->Exce();
}

//速度と加速度を位置に適応
void Enemy::IntegrateMotion() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//加速度を適応
		entityGroup_.entity[i].gameObject.velocity += entityGroup_.entity[i].gameObject.acceleration * Math::kDeltaTime;
		//速度を適応
		entityGroup_.entity[i].gameObject.transformData.translate += entityGroup_.entity[i].gameObject.velocity * Math::kDeltaTime;
	}
}
