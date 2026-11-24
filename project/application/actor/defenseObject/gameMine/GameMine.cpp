#include "GameMine.h"

//コンストラクタ
GameMine::GameMine(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
GameMine::~GameMine() {
}

//初期化
void GameMine::Initialize() {
}

//更新
void GameMine::Update() {
}

//複製
std::unique_ptr<Component> GameMine::Clone(GameObject* gameObject) const {
	std::unique_ptr<GameMine>cloneInstance = std::make_unique<GameMine>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string GameMine::GetTypeName() const {
	return "GameMine";
}