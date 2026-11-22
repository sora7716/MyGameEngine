#include "Wall.h"

//コンストラクタ
Wall::Wall(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
Wall::~Wall() {
}

//初期化
void Wall::Initialize() {
}

//更新
void Wall::Update() {
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