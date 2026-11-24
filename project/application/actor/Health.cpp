#define NOMINMAX
#include "Health.h"
#include <algorithm>

//コンストラクタ
Health::Health(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Health::~Health() {
}

//更新
void Health::Update() {
}

//HPからダメージ分減らす処理
void Health::ApplyDamage(float damage) {
	//現在のHPをダメージ分引く
	currentHp_ -= damage;
	//現在のHPがゼロに行かないようにする
	currentHp_ = std::max(currentHp_,0.0f);
}

//HPを回復させる
void Health::Heal(float heal) {
	//現在のHPを回復させる
	currentHp_ += heal;
	//現在のHPがHPの最大値を超えないようにする
	currentHp_ = std::min(currentHp_,maxHp_);
}

//プロパティ一覧を登録
void Health::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("現在のHP",currentHp_);
	registry->Add("最大HP",maxHp_);
}

//複製
std::unique_ptr<Component> Health::Clone(GameObject* gameObject) const {
	std::unique_ptr<Health>cloneInstance = std::make_unique<Health>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->maxHp_ = this->maxHp_;
	cloneInstance->currentHp_ = this->currentHp_;

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string Health::GetTypeName() const {
	return "Health";
}

//HPの最大値を設定
void Health::SetMaxHp(float maxHp) {
	maxHp_ = std::max(maxHp,0.0f);
	//現在のHPを満タンにする
	currentHp_ = maxHp_;
}

//死亡フラグの取得
bool Health::IsDead() const {
	//現在のHPが0以下だった場合
	return currentHp_ <= 0.0f;
}

//現在のHPを取得
float Health::GetCurrentHp() const {
	return currentHp_;
}

//最大のHPを取得
float Health::GetMaxHp() const {
	return maxHp_;
}
