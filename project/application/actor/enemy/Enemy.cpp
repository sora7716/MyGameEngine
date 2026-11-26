#include "Enemy.h"
#include "GameObject.h"
#include "Health.h"
#include "Logger.h"
#include "BaseCollider.h"
#include "MathUtility.h"
#include "ImGuiManager.h"
#include <cassert>

//コンストラクタ
Enemy::Enemy(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Enemy::~Enemy() {
}

//初期化
void Enemy::Initialize() {
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
}

//更新
void Enemy::Update() {
	//死亡処理
	OnDead();

	//ショックのステータスが存在しているか
	if(HasStatus(EnemyStatus::kShock)) {
		if(behavior_ != Behavior::kShock) {
			behaviorRequest_ = Behavior::kShock;
		}
	}

	//振る舞いの要求があった場合
	if(behaviorRequest_ != Behavior::kCount) {
		//要求をコピー
		behavior_ = behaviorRequest_;
		//要求をリセット
		behaviorRequest_ = Behavior::kCount;
		//振る舞いの初期化
		switch(behavior_) {
			case Behavior::kShock:
				InitializeShockState();
				break;
			case Behavior::kDead:
				OnDead();
				break;
		}
	}

	//振る舞いの要求
	switch(behavior_) {
		case Enemy::Behavior::kShock:
			UpdateShockState();
			break;
	}
}

//デバッグでImGuiを使えるようにする
void Enemy::DebugImGui() {
	ImGui::Begin("EnemyState");
	ImGui::Text("敵のショック状態:%f",shockState_.elapsedTime_);
	ImGui::Text("敵のダメージを受けるタイミング:%f",shockState_.tickTimer_);
	ImGui::End();
}

//プロパティ一覧を登録
void Enemy::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("敵の攻撃力",attackPower_);
}

//接触した瞬間
void Enemy::OnCollisionEnter(const CollisionInfo& info) {
	//ゲームオブジェクト
	GameObject* gameObject = info.other->GetOwner();
	if(gameObject->GetTag() == "DefenseObject") {
		//HPの管理を取得
		Health* health = gameObject->GetComponent<Health>();
		//HPを減らす
		if(health) {
			health->ApplyDamage(attackPower_);
		}
	}
}

//接触した瞬間
void Enemy::OnTriggerEnter(BaseCollider* other) {
	//ゲームオブジェクト
	GameObject* gameObject = other->GetOwner();
	if(gameObject->GetTag() == "LightningStrike") {
		//現在のステータスにショックを追加
		status_ = status_ | EnemyStatus::kShock;
		//ショックステートの継続時間をリセット
		shockState_.elapsedTime_ = 0.0f;
	}
}

//複製
std::unique_ptr<Component> Enemy::Clone(GameObject* gameObject) const {
	std::unique_ptr<Enemy>cloneInstance = std::make_unique<Enemy>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string Enemy::GetTypeName() const {
	return "Enemy";
}

//ショックパラメータを設定
void Enemy::SetShockParameter(const ShockParameter& parameter) {
	shockParameter_ = parameter;
}

//ステータスが存在しいるか取得
bool Enemy::HasStatus(EnemyStatus status) {
	return (status_ & status) == status;
}

//死亡処理
void Enemy::OnDead() {
	//自分のHPの状態を取得
	Health* health = gameObject_->GetComponent<Health>();
	if(!health) {
		Logger::OutputLog("Healthのコンポーネントが取得できませんでした");
		assert(false);
		return;
	}
	//自分の死亡フラグが立っていたら
	if(health->IsDead()) {
		gameObject_->SetIsActive(false);
	}
}

//ショック状態の初期化
void Enemy::InitializeShockState() {
	//ショックステータスの初期化
	shockState_ = {};
}

//ショック状態の更新
void Enemy::UpdateShockState() {
	//ショックの継続時間
	shockState_.elapsedTime_ += mathUtility::GetDeltaTime();

	//ショックの継続時間がなくなったら
	if(shockState_.elapsedTime_ >= shockParameter_.maxDuration) {
		behaviorRequest_ = Behavior::kRoot;
		//ステータスからショックの状態を除去
		status_ = status_ & ~EnemyStatus::kShock;
		return;
	}

	//スタンやダメージを受けるタイミングを計測
	shockState_.tickTimer_ += mathUtility::GetDeltaTime();
	//タイミングがまだ来てないなら
	if(shockState_.tickTimer_ < shockParameter_.tickiInterval) {
		return;
	}

	//タイミングが来たら
	//タイミングをリセット
	shockState_.tickTimer_ = 0.0f;
	//ダメージを受ける
	Health* health = gameObject_->GetComponent<Health>();
	if(!health) {
		Logger::OutputLog("Healthが取得できませんでした");
		assert(false);
		return;
	}
	health->ApplyDamage(shockParameter_.damage);

	//移動を止める
	shockState_.isStunned = true;
}
