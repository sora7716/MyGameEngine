#include "MapChipCollider.h"
#include "Logger.h"
//コンストラクタ
MapChipCollider::MapChipCollider(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
MapChipCollider::~MapChipCollider() {
}

//セットアップ
void MapChipCollider::Setup(const MapChipData& mapChip) {
	//マップチップのデータが空だった場合
	if(mapChip.data.empty()) {
		Logger::OutputLog("MapChipColliderにMapChipDataが空の状態でSetupが呼ばれました");
		return;
	}
	//マップチップを記録
	mapChip_ = mapChip;
}

//初期化
void MapChipCollider::Initialize() {
	//ゲームオブジェクトの取得
	gameObject_ = GetOwner();
}

//更新
void MapChipCollider::Update() {

}

//複製
std::unique_ptr<Component> MapChipCollider::Clone(GameObject* gameObject) const {
	std::unique_ptr<MapChipCollider>cloneInstance = std::make_unique<MapChipCollider>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を取得
std::string MapChipCollider::GetTypeName() const {
	return "MapChipCollider";
}
