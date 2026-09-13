#include "Enemy.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "MathUtility.h"

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
}

//更新
void Enemy::Update(){
	//振る舞いのリクエストがNoneじゃない場合
	if (behaviorRequest_ != Behavior::kNone){
		//リクエストをコピー
		behavior_ = behaviorRequest_;
		//リクエストをリセット
		behaviorRequest_ = Behavior::kNone;

		//振る舞いを変更する
		switch (behavior_){
		case Enemy::Behavior::kNormal:
			//通常状態の初期化
			RootInitialize();
			break;
		case Enemy::Behavior::kDamage:
			//のけぞり状態の初期化
			HitReactionInitialize();
			break;
		}
	}

	//振る舞いを変更する
	switch (behavior_){
	case Enemy::Behavior::kNormal:
		//通常状態の更新
		RootUpdate();
		break;
	case Enemy::Behavior::kDamage:
		//のけぞり状態の更新
		HitReactionUpdate();
		break;
	}
}

//衝突したら
void Enemy::OnTrigger(BaseCollider* other){
	//自分の位置
	const Vector3& myPosition = gameObject_->GetTransform().translate;
	//衝突対象の位置
	const Vector3& otherPosition = other->GetOwner()->GetTransform().translate;
	//衝突した方向を取得
	hitDirection_ = myPosition - otherPosition;

	//衝突したのがプレイヤーだったら
	if (other->GetOwner()->GetTag() == "Player"){
		behaviorRequest_ = Behavior::kDamage;
	}
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
void Enemy::HitReactionInitialize(){
	//のけぞりタイマーの初期化
	reactionTimer_ = 0.0f;

	//衝突した方向のY軸を0にし水平面だけを見る
	hitDirection_.y = 0.0f;
	//方向を正規化
	hitDirection_ = hitDirection_.Normalize();

	//上方向のベクトルと衝突した方向ベクトルでクロス積を行い回転軸を求める
	Vector3 rotateAxis = hitDirection_.Cross(Vector3::GetUp());

	//のけぞる姿勢(クォータニオン)を取得
	reactionQuaternion_ = Quaternion::MakeRotateAxisAngleQuaternion(rotateAxis, kReactionAngle * mathUtility::kRad);
}

//のけぞりアクションの更新
void Enemy::HitReactionUpdate(){
	//のけぞる姿勢を適応(Slerp)
	Quaternion& rotate = gameObject_->GetTransform().quaternion;
	//リアクションタイマーを加算
	reactionTimer_ += mathUtility::kDeltaTime;
	//係数を取得
	float t = reactionTimer_ / kReactionDuration;
	//Slerp
	rotate = Quaternion::Slerp(rotate, reactionQuaternion_, t);

	//最大時間を過ぎたらフェーズを変更
	if (reactionTimer_ > kReactionDuration){
		damagePhase_ = DamagePhase::kKnockback;
	}
}

//ノックバックの初期化
void Enemy::KnockbackInitialize(){
	//タイマーのリセット
	knockbackTimer_ = 0.0f;
	//吹き飛ばす
}

//ノックバックの更新
void Enemy::KnockbackUpdate(){
}

//浮き上がるときの初期化
void Enemy::RecoverInitialize(){
}

//浮き上がるときの更新
void Enemy::RecoverUpdate(){
}
