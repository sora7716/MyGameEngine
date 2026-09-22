#include "EnemyDamageState.h"
#include "mathUtility.h"
#include "Enemy.h"
#include "RigidBody.h"
#include "GameObject.h"
#include <cassert>

//コンストラクタ
EnemyDamageState::EnemyDamageState(){
}

//デストラクタ
EnemyDamageState::~EnemyDamageState(){
}

//初期化
void EnemyDamageState::Initialize(Enemy* enemy){
	//敵を記録
	assert(enemy);
	enemy_ = enemy;

	//ゲームオブジェクトを取得
	gameObject_ = enemy_->GetOwner();
	//リジットボディを取得
	rigidBody_ = enemy_->GetRigidBody();
	//衝突した方向を取得
	hitDirection_ = enemy_->GetHitDirection();
	//ダメージを受ける前の回転
	normalRotate_ = enemy_->GetNormalRotate();

	//のけぞりの初期化
	InitializeFlinch();
	//ノックバックの初期化
	InitializeKnockback();
}

//更新
void EnemyDamageState::Update(){
	//のけぞりの更新
	UpdateFlinch();
	//ノックバックの更新
	UpdateKnockback();

	//のけぞりとノックバックが終了したら
	if (isFinishedFlinch_ && isFinishedKnockback_){
		enemy_->SetBehaviorRequest(Enemy::Behavior::kRecover);
	}
}

//終了
void EnemyDamageState::Finalize(){
}

//のけぞりアクションの初期化
void EnemyDamageState::InitializeFlinch(){
	//のけぞりタイマーの初期化
	flinchTimer_ = 0.0f;
	//終了フラグをリセット
	isFinishedFlinch_ = false;

	//衝突した方向のY軸を0にし水平面だけを見る
	hitDirection_.y = 0.0f;
	//方向を正規化
	hitDirection_ = hitDirection_.Normalize();

	//上方向のベクトルと衝突した方向ベクトルでクロス積を行い回転軸を求める
	Vector3 rotateAxis = Vector3::GetUp().Cross(hitDirection_);

	//のけぞる姿勢(クォータニオン)を取得
	flinchRotate_ = Quaternion::MakeRotateAxisAngleQuaternion(rotateAxis, kFlinchAngle * mathUtility::kRad);
}

//のけぞりアクションの更新
void EnemyDamageState::UpdateFlinch(){
	//終了フラグがtrueなら
	if (isFinishedFlinch_){
		return;
	}

	//のけぞる姿勢を適応(Slerp)
	Quaternion& rotate = gameObject_->GetTransform().rotate;
	//リアクションタイマーを加算
	flinchTimer_ += mathUtility::kDeltaTime;
	//係数を取得
	float t = flinchTimer_ / kFlinchDuration;

	//Slerp
	rotate = Quaternion::Slerp(normalRotate_, flinchRotate_, t);

	//タイマーが過ぎたら
	if (flinchTimer_ >= kFlinchDuration){
		//終了を通知
		isFinishedFlinch_ = true;
		return;
	}
}

//ノックバックの初期化
void EnemyDamageState::InitializeKnockback(){
	//タイマーのリセット
	knockbackTimer_ = 0.0f;
	//終了フラグをリセット
	isFinishedKnockback_ = false;

	//衝突した方向のY軸を0にし水平面だけを見る
	hitDirection_.y = 0.0f;
	//正規化する
	hitDirection_ = hitDirection_.Normalize();

	//速度を求める
	rigidBody_->GetVelocity() = hitDirection_ * kKnockbackSpeed;
}

//ノックバックの更新
void EnemyDamageState::UpdateKnockback(){
	//終了フラグがtrueなら
	if (isFinishedKnockback_){
		return;
	}

	//タイマーを計測
	knockbackTimer_ += mathUtility::kDeltaTime;

	//減速を求める
	rigidBody_->GetAcceleration() = -hitDirection_ * (kKnockbackSpeed / kKnockbackDuration);

	//タイマーが過ぎたら
	if (knockbackTimer_ >= kKnockbackDuration){
		//速度と加速度をリセット
		rigidBody_->GetVelocity() = Vector3::GetZero();
		rigidBody_->GetAcceleration() = Vector3::GetZero();

		//終了を通知
		isFinishedKnockback_ = true;
	}
}