#include "Indicator.h"

//コンストラクタ
Indicator::Indicator(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
Indicator::~Indicator() {
}

//更新
void Indicator::Update() {
}

//接触した瞬間
void Indicator::OnTriggerEnter(BaseCollider* other) {
}

//離れた瞬間
void Indicator::OnTriggerExit(BaseCollider* other) {
}

//複製
std::unique_ptr<Component> Indicator::Clone(GameObject* gameObject) const {
	std::unique_ptr<Indicator>cloneInstance = std::make_unique<Indicator>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string Indicator::GetTypeName() const {
	return "Indicator";
}