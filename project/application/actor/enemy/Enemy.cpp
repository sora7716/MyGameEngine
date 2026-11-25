#include "Enemy.h"
#include "GameObject.h"
#include "Health.h"
#include "Logger.h"
#include "BaseCollider.h"
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