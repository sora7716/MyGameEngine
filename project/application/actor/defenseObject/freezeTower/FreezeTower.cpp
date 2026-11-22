#include "FreezeTower.h"

//コンストラクタ
FreezeTower::FreezeTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
FreezeTower::~FreezeTower() {
}

//初期化
void FreezeTower::Initialize() {
}

//更新
void FreezeTower::Update() {
}

//複製
std::unique_ptr<Component> FreezeTower::Clone(GameObject* gameObject) const {
	std::unique_ptr<FreezeTower>cloneInstance = std::make_unique<FreezeTower>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string FreezeTower::GetTypeName() const {
	return "FreezeTower";
}