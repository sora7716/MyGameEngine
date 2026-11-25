#include "Wall.h"
#include "Health.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
Wall::Wall(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
Wall::~Wall() {
}

//初期化
void Wall::Initialize() {
	//ゲームオブジェクトの取得
	gameObject_ = GetOwner();
}

//更新
void Wall::Update() {
	//死亡
	CheckDeath();

	//自己回復
	SelfHeal();
}

//プロパティ一覧を登録
void Wall::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("回復量",healAmount_);
	registry->Add("回復間隔",healInterval_);
	registry->Add("次の回復まで",healTimer_);
}

//複製
std::unique_ptr<Component> Wall::Clone(GameObject* gameObject) const {
	std::unique_ptr<Wall>cloneInstance = std::make_unique<Wall>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string Wall::GetTypeName() const {
	return "Wall";
}

//回復量を設定
void Wall::SetHealAmount(float healAmount) {
	healAmount_ = healAmount;
}

//死亡したか確認
void Wall::CheckDeath() {
	//死亡フラグがたったら
	Health* health = gameObject_->GetComponent<Health>();
	//死亡してない場合
	if(!health->IsDead()) {
		return;
	}
	//死亡していたら
	gameObject_->SetIsActive(false);
}

//自己回復
void Wall::SelfHeal() {
	//時間を計測
	healTimer_ += mathUtility::GetDeltaTime();

	//今の時間がインターバル以上なら
	if(healTimer_ >= healInterval_) {
		//HPの管理を取得
		Health* health = gameObject_->GetComponent<Health>();
		//体力が減っていなかった場合
		if(health->GetCurrentHp() >= health->GetMaxHp()) {
			return;
		}

		//体力が減っていた場合
		if(!health) {
			Logger::OutputLog("Healthが取得できませんでした");
			assert(false);
			return;
		}

		//今の時間をリセット
		healTimer_ = 0.0f;
		//回復させる
		health->Heal(healAmount_);
	}
}
