#include "Enemy.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "MathUtility.h"
#include "RigidBody.h"

//コンストラクタ
Enemy::Enemy(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
Enemy::~Enemy(){
}

//初期化
void Enemy::Initialize(){
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
	//リジッドボディを取得
	rigidBody_ = gameObject_->GetComponent<RigidBody>();
}

//更新
void Enemy::Update(){
	//振る舞いのリクエストがNoneじゃない場合
	if (behaviorRequest_ != Behavior::kNone){
		//リクエストをコピー
		behavior_ = behaviorRequest_;
		//リクエストをリセット
		behaviorRequest_ = Behavior::kNone;

		//振る舞いの初期化
		switch (behavior_){
		case Enemy::Behavior::kNormal:
			//通常状態の初期化
			RootInitialize();
			break;
		}
	}

	//振る舞いの更新
	switch (behavior_){
	case Enemy::Behavior::kNormal:
		//通常状態の更新
		RootUpdate();
		break;
	case Enemy::Behavior::kDamage:
		//ダメージフェーズのリクエストがNoneじゃない場合
		if (damagePhaseRequest_ != DamagePhase::kNone){
			//リクエストをコピー
			damagePhase_ = damagePhaseRequest_;
			//リクエストをリセット
			damagePhaseRequest_ = DamagePhase::kNone;

			//ダメージフェーズの初期化
			switch (damagePhase_){
			case DamagePhase::kDamageReaction:
				//ダメージリアクションの初期化
				DamageReactionInitialize();
				break;
			case DamagePhase::kRecover:
				//元に戻すための初期化
				RecoverInitialize();
				break;
			}
		}

		//ダメージフェーズの更新
		switch (damagePhase_){
		case DamagePhase::kDamageReaction:
			//ダメージリアクションの更新
			DamageReactionUpdate();
			break;
		case DamagePhase::kRecover:
			//元に戻すための更新
			RecoverUpdate();
			break;
		}
		break;
	}
}

//衝突瞬間
void Enemy::OnTriggerStay(BaseCollider* other){
	//振る舞いを変更
	behaviorRequest_ = Behavior::kDamage;

	//自分の位置
	const Vector3& myPosition = gameObject_->GetTransform().translate;
	//衝突対象の位置
	const Vector3& otherPosition = other->GetOwner()->GetTransform().translate;
	//衝突した方向を取得
	hitDirection_ = myPosition - otherPosition;
}

//コピー
std::unique_ptr<Component> Enemy::Clone(GameObject* gameObject) const{
	std::unique_ptr<Enemy>cloneInstance = std::make_unique<Enemy>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//通常状態の初期化
void Enemy::RootInitialize(){
}

//通常状態の更新
void Enemy::RootUpdate(){
}

//のけぞりアクションの初期化
void Enemy::FlinchInitialize(){
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
void Enemy::FlinchUpdate(){
	//終了フラグがtrueなら
	if (isFinishedFlinch_){
		return;
	}

	//のけぞる姿勢を適応(Slerp)
	Quaternion& rotate = gameObject_->GetTransform().quaternion;
	//リアクションタイマーを加算
	flinchTimer_ += mathUtility::kDeltaTime;
	//係数を取得
	float t = flinchTimer_ / kFlinchDuration;

	//Slerp
	rotate = Quaternion::Slerp(damageReactionStartRotate_, flinchRotate_, t);

	//タイマーが過ぎたら
	if (flinchTimer_ >= kFlinchDuration){
		//終了を通知
		isFinishedFlinch_ = true;
		return;
	}
}

//ノックバックの初期化
void Enemy::KnockbackInitialize(){
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
void Enemy::KnockbackUpdate(){
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
		rigidBody_->GetVelocity() = Vector3::Zero();
		rigidBody_->GetAcceleration() = Vector3::Zero();

		//終了を通知
		isFinishedKnockback_ = true;
	}
}

//ダメージリアクションの初期化
void Enemy::DamageReactionInitialize(){
	//ダメージリアクションする前の回転
	damageReactionStartRotate_ = gameObject_->GetTransform().quaternion;

	//のけぞりの初期化
	FlinchInitialize();
	//ノックバックの初期化
	KnockbackInitialize();
}

//ダメージリアクションの更新
void Enemy::DamageReactionUpdate(){
	//のけぞりの更新
	FlinchUpdate();
	//ノックバックの更新
	KnockbackUpdate();

	//のけぞりとノックバックが終了したら
	if (isFinishedFlinch_ && isFinishedKnockback_){
		damagePhaseRequest_ = DamagePhase::kRecover;
	}
}

//浮き上がるときの初期化
void Enemy::RecoverInitialize(){
	//タイマーのリセット
	recoverTimer_ = 0.0f;
	//元に戻す瞬間の回転
	flinchRotate_ = gameObject_->GetTransform().quaternion;
}

//浮き上がるときの更新
void Enemy::RecoverUpdate(){
	//タイマーの加算
	recoverTimer_ += mathUtility::kDeltaTime;

	//係数を求める
	float t = recoverTimer_ / kRecoverDuration;

	//元に戻す
	Quaternion& rotate = gameObject_->GetTransform().quaternion;
	//補間する
	rotate = rotate.Slerp(flinchRotate_, damageReactionStartRotate_, t);

	//時間が過ぎたら
	if (recoverTimer_ >= kRecoverDuration){
		//振る舞いのリクエストを送信
		behaviorRequest_ = Behavior::kNormal;

		//ダメージフェーズのリクエストを送信
		damagePhaseRequest_ = DamagePhase::kDamageReaction;
	}
}
