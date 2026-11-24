#include "ThunderTower.h"

//コンストラクタ
ThunderTower::ThunderTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
ThunderTower::~ThunderTower() {
}

//初期化
void ThunderTower::Initialize() {
}

//更新
void ThunderTower::Update() {
}

//複製
std::unique_ptr<Component> ThunderTower::Clone(GameObject* gameObject) const {
	std::unique_ptr<ThunderTower>cloneInstance = std::make_unique<ThunderTower>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string ThunderTower::GetTypeName() const {
	return "ThunderTower";
}