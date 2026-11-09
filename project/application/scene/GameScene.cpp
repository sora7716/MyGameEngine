#include "GameScene.h"
#include "GameObject.h"
#include "SkyBox.h"
#include "MapChipField.h"
#include "Object3d.h"
#include "DirectionalLight.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Player.h"

//コンストラクタ
GameScene::GameScene() {
}

//デストラクタ
GameScene::~GameScene() {
}

//初期化
void GameScene::Initialize() {
	//基底クラスの初期化
	BaseScene::Initialize();

	//平行光源
	GameObject* gameObject = CreateGameObject();
	gameObject->AddComponent<DirectionalLight>();
	gameObject->GetTransform().SetEulerAngleDegrees({-120.0f,0.0f,0.0f});
	gameObject->SetName("平行光源");

	//SkyBoxの生成
	CreateSkyBox();

	//フィールドの生成
	CreateField();

	//プレイヤーの作成
	CreatePlayer();
}

//更新のステート
void GameScene::UpdateState() {
}

//デバッグ
void GameScene::Debug() {
	//基底クラスのデバッグ
	BaseScene::Debug();
}

//解放処理
void GameScene::Finalize() {
	//基底クラスの解放
	BaseScene::Finalize();
}

//SkyBoxの生成
GameObject* GameScene::CreateSkyBox() {
	//ゲームオブジェクト
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().scale = {100.0f,100.0f,100.0f};
	gameObject->SetName("SkyBox");
	gameObject->SetTag("SkyBox");

	//SkyBox
	gameObject->AddComponent<SkyBox>();

	return gameObject;
}

//フィールドの生成
void GameScene::CreateField() {
	std::unique_ptr<MapChipField> mapChipField = std::make_unique<MapChipField>();
	mapChipField->LoadMapChipCsv(mapChipCsvName_);
	//マップチップのデータ
	MapChipData mapChipData = mapChipField->GetMapChipData();

	//フィールド
	GameObject* field = CreateGameObject();
	field->SetName("フィールド");

	//各ブロックを生成
	for(uint32_t z = 0; z < mapChipData.data.size(); z++) {
		std::vector<MapChipStatus>mapChipStatusData = mapChipData.data[z];
		for(uint32_t x = 0; x < mapChipStatusData.size(); x++) {
			//ステータス
			MapChipStatus status = mapChipStatusData[x];
			for(uint32_t y = 0; y < status.count; y++) {
				//空白だった場合
				if(status.type == MapChipType::kBlank) {
					continue;
				}

				//フィールドの半径
				float radiusX = static_cast<float>(mapChipData.data.size()) * MapChipField::kBlockWidth / 2.0f;
				float radiusZ = static_cast<float>(mapChipStatusData.size()) * MapChipField::kBlockWidth / 2.0f;

				//ゲームオブジェクトの生成
				GameObject* gameObject = CreateGameObject();
				gameObject->AttachTo(field);
				gameObject->SetName("Block" + std::to_string(z) + "-" + std::to_string(x));
				gameObject->GetTransform().translate = {
					static_cast<float>(x) * MapChipField::kBlockWidth - radiusX,
					static_cast<float>(y) * MapChipField::kBlockHeight,
					static_cast<float>(z) * MapChipField::kBlockDepth - radiusZ
				};
				Object3d* object3d = gameObject->AddComponent<Object3d>();
				object3d->SetModel("cube");
			}
		}
	}
}

//プレイヤーの作成
void GameScene::CreatePlayer() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("プレイヤー");
	//オブジェクト3dを設定
	Object3d*object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("player");
	//リジッドボディを設定
	RigidBody* rigidBody = gameObject->AddComponent<RigidBody>();
	//プレイヤーの設定
	Player* player = gameObject->AddComponent<Player>();
	player->SetObject3d(object3d);
	player->SetInput(GetSceneContext().input);
	player->SetRigidBody(rigidBody);
}
