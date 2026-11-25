#define NOMINMAX
#include "Health.h"
#include "MathUtility.h"
#include <algorithm>

//コンストラクタ
Health::Health(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Health::~Health() {
}

//更新
void Health::Update() {
	//最終的なHPと今のHPの差分
	float diff = targetHp_ - currentHp_;
	//1フレームで動貸したい値
	float stepAmount = (maxHp_ / hpDuration_) * mathUtility::GetDeltaTime();
	//現在のHPを補間する
	currentHp_ += std::clamp(diff,-stepAmount,stepAmount);
}

//HPからダメージ分減らす処理
void Health::ApplyDamage(float damage) {
	//最終的に減るHP
	targetHp_ = targetHp_ - damage;
	//最終的に減るHPがゼロに行かないようにする
	targetHp_ = std::max(targetHp_,0.0f);
}

//HPを回復させる
void Health::Heal(float heal) {
	//最終的に回復するHP
	targetHp_ = targetHp_ + heal;
	//現在のHPがHPの最大値を超えないようにする
	targetHp_ = std::min(targetHp_,maxHp_);
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
	//最終的はHPも満タンにする
	targetHp_ = maxHp_;
}

//HPが減り切る秒数を設定
void Health::SetHpDuration(float duration) {
	hpDuration_ = std::max(duration,mathUtility::kEpsilon);
}

//死亡フラグの取得
bool Health::IsDead() const {
	//現在のHPが0以下だった場合
	return currentHp_ <= 0.1f;
}

//現在のHPを取得
float Health::GetCurrentHp() const {
	return currentHp_;
}

//最大のHPを取得
float Health::GetMaxHp() const {
	return maxHp_;
}
