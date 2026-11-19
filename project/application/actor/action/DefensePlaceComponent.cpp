#include "DefensePlaceComponent.h"

//コンストラクタ
DefensePlaceComponent::DefensePlaceComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
DefensePlaceComponent::~DefensePlaceComponent() {
}

//初期化
void DefensePlaceComponent::Initialize() {
}

//更新
void DefensePlaceComponent::Update() {
}

//プロパティ一覧
void DefensePlaceComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
}

//複製
std::unique_ptr<Component> DefensePlaceComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<DefensePlaceComponent>cloneInstance = std::make_unique<DefensePlaceComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->confirmRequest_ = this->confirmRequest_;

	return cloneInstance;
}

//コンポーネントタイプ
std::string DefensePlaceComponent::GetTypeName() const {
	return "DefensePlaceComponent";
}

//設置要求の設定
void DefensePlaceComponent::SetConfirmRequest(bool confirmRequest) {
	confirmRequest_ = confirmRequest;
}

//設置要求の取得
bool DefensePlaceComponent::GetConfirmRequest() const {
	return confirmRequest_;
}
