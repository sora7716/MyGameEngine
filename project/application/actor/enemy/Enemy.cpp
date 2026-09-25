#include "Enemy.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "MathUtility.h"
#include "RigidBody.h"
#include "EnemyNormalState.h"
#include "EnemyDamageState.h"
#include "EnemyRecoverState.h"

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

	//通常の回転を保存
	normalRotate_ = gameObject_->GetTransform().rotate;

	//ステートを取得
	states_[static_cast<uint32_t>(Behavior::kNormal)] = std::make_unique<EnemyNormalState>();
	states_[static_cast<uint32_t>(Behavior::kDamage)] = std::make_unique<EnemyDamageState>();
	states_[static_cast<uint32_t>(Behavior::kRecover)] = std::make_unique <EnemyRecoverState>();
}

//更新
void Enemy::Update(){
	//振る舞いのリクエストがNoneじゃない場合
	if (behaviorRequest_ != Behavior::kCount){
		//リクエストをコピー
		behavior_ = behaviorRequest_;
		//リクエストをリセット
		behaviorRequest_ = Behavior::kCount;

		//ステートを切り替え
		ChangeState(behavior_);
	}

	//ステートの更新
	currentState_->Update();
}

//衝突瞬間
void Enemy::OnTriggerEnter(BaseCollider* other){
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

//振る舞いのリクエストの設定
void Enemy::SetBehaviorRequest(Behavior request){
	behaviorRequest_ = request;
}

//リジットボディの取得
RigidBody* Enemy::GetRigidBody(){
	return rigidBody_;
}

//衝突した方向ベクトルの取得
const Vector3& Enemy::GetHitDirection() const{
	return hitDirection_;
}

//ダメージを受けた瞬間の回転を取得
const Quaternion& Enemy::GetNormalRotate() const{
	return normalRotate_;
}

//ステートの切り替え
void Enemy::ChangeState(Behavior behavior){
	//次のステートを取得
	IEnemyState* nextState = states_[static_cast<uint32_t>(behavior)].get();

	//今のステートを終了
	if (currentState_){
		currentState_->Exit();
	}

	//今のステートを次にステートへ変更
	currentState_ = nextState;
	currentState_->Enter(this);
}
