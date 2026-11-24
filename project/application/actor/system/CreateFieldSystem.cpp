#include "CreateFieldSystem.h"
#include "BaseScene.h"
#include "MapChipField.h"
#include "GameObject.h"
#include "Object3d.h"
#include <cassert>
using namespace mapChipDesc;

//生成
std::unique_ptr<CreateFieldSystem> CreateFieldSystem::Create(BaseScene* scene,const std::string& mapChipCsvName) {
	std::unique_ptr<CreateFieldSystem>instance = std::make_unique<CreateFieldSystem>();
	instance->Initialize(scene,mapChipCsvName);
	return instance;
}

//コンストラクタ
CreateFieldSystem::CreateFieldSystem() {
}

//デストラクタ
CreateFieldSystem::~CreateFieldSystem() {
}

//初期化
void CreateFieldSystem::Initialize(BaseScene* scene,const std::string& mapChipCsvName) {
	//シーンの記録
	assert(scene);
	scene_ = scene;
	//マップチップのCSVファイルの記録
	mapChipCsvName_ = mapChipCsvName;
	//マップチップフィールドの生成
	mapChipField_ = MapChipField::Create(mapChipCsvName_);
	//マップの生成
	CreateField();

}

//マップチップフィールドの取得
MapChipField* CreateFieldSystem::GetMapChipField() {
	return mapChipField_.get();
}

//フィールドブロックデータの取得
const FieldBlockData& CreateFieldSystem::GetFieldBlockData() const {
	return fieldBlockData_;
}

//フィールドの生成
void CreateFieldSystem::CreateField() {
	//マップチップのデータ
	MapChipData mapChipData = mapChipField_->GetMapChipData();

	//フィールド
	GameObject* field = scene_->CreateGameObject();
	field->SetName("フィールド");
	//フィールドブロックの配列サイズを設定
	fieldBlockData_.data.resize(mapChipData.data.size());
	//各ブロックを生成
	for(uint32_t z = 0; z < mapChipData.data.size(); z++) {
		//フィールドブロックの配列サイズを設定
		fieldBlockData_.data[z].resize(mapChipData.data[z].size());
		//ステータスのデータを取得
		std::vector<MapChipStatus>mapChipStatusData = mapChipData.data[z];
		for(uint32_t x = 0; x < mapChipStatusData.size(); x++) {
			//ステータス
			MapChipStatus status = mapChipStatusData[x];
			//フィールドブロックの配列サイズを設定
			fieldBlockData_.data[z][x].resize(status.height);
			for(uint32_t y = 0; y < status.height; y++) {
				//空白だった場合
				if(status.type == MapChipType::kBlank) {
					fieldBlockData_.data[z][x][y] = nullptr;
					continue;
				}

				//ゲームオブジェクトの生成
				GameObject* gameObject = scene_->CreateGameObject();
				gameObject->AttachTo(field);
				gameObject->SetName("Block" + std::to_string(z) + "-" + std::to_string(x));
				Vector3Int positionIndex = {
					static_cast<int32_t>(x),
					static_cast<int32_t>(y),
					static_cast<int32_t>(z)
				};
				gameObject->GetTransform().translate = mapChipField_->GetMapChipPositionByIndex(positionIndex);
				Object3d* object3d = gameObject->AddComponent<Object3d>();
				object3d->SetModel("block");

				fieldBlockData_.data[z][x][y] = gameObject;
			}
		}
	}
}