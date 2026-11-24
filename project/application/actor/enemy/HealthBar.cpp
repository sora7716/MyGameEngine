#include "HealthBar.h"
#include "GameObject.h"
#include "Health.h"
#include <cassert>

//コンストラクタ
HealthBar::HealthBar(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
HealthBar::~HealthBar() {
}

//セットアップ
void HealthBar::Setup(Health* health) {
	//HPの管理を記録
	assert(health);
	health_ = health;
}

//初期化
void HealthBar::Initialize() {
	Transform& transform = GetOwner()->GetTransform();
	//サイズを設定
	transform.scale.y = 0.25f;

	//位置を設定
	transform.translate.x = -(kBarMeshWidth * fullBarScale) / 2.0f;
	transform.translate.y = 0.5f;
}

//更新
void HealthBar::Update() {
	Transform& transform = GetOwner()->GetTransform();
	//サイズを設定
	float hpRation = health_->GetCurrentHp() / health_->GetMaxHp();
	transform.scale.x = fullBarScale * hpRation;
}

//プロパティ一覧を登録
void HealthBar::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("HPのマックスのときのScale",fullBarScale);
}

//複製
std::unique_ptr<Component> HealthBar::Clone(GameObject* gameObject) const {
	std::unique_ptr<HealthBar>cloneInstance = std::make_unique<HealthBar>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string HealthBar::GetTypeName() const {
	return "HealthBar";
}

//満タン時のスケールを取得
float HealthBar::GetFullBarScale() {
	return fullBarScale;
}
