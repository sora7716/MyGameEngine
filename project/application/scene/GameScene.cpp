#include "GameScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Player.h"
#include "SkyBox.h"

//コンストラクタ
GameScene::GameScene(){
}

//デストラクタ
GameScene::~GameScene(){
}

//初期化
void GameScene::Initialize(){
	//基底クラスの初期化
	BaseScene::Initialize();
	//ゲームカメラの設定
	GameObject* gameCameraObject = CreateGameObject();
	gameCameraObject->AddComponent<Camera>();
	gameCameraObject->GetTransform().SetEulerAngle({ 0.4f,0.0f,0.0f });
	gameCameraObject->GetTransform().translate = { 2.4f,7.9f,-14.1f };
	gameCameraObject->SetName("ゲームカメラ");

	//SkyBox
	GameObject* skyBoxObject = CreateGameObject();
	skyBoxObject->AddComponent<SkyBox>();
	skyBoxObject->GetTransform().scale = { 100.0f,100.0f,100.0f };
	skyBoxObject->SetName("SkyBox");

	//マップを作成
	std::array<std::array<std::array<uint32_t, 6>, 6>, 3> map = { {
		{{
			{{1, 1, 1, 1, 1, 1}},
			{{1, 1, 1, 1, 1, 1}},
			{{1, 1, 1, 1, 1, 1}},
			{{1, 1, 1, 1, 1, 1}},
			{{1, 1, 1, 1, 1, 1}},
			{{1, 1, 1, 1, 1, 1}}
		}},
		{{
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{1, 0, 0, 0, 0, 0}}
		}},
		{{
			{{0, 0, 0, 0, 0, 1}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{1, 0, 0, 0, 0, 1}}
		}},
	} };

	//マップの生成
	std::vector<std::vector<std::vector<GameObject*>>>mapBlocks;
	mapBlocks.resize(kMapSize.y);
	for (std::vector<std::vector<GameObject*>>& plane : mapBlocks){
		plane.resize(kMapSize.z);
		for (std::vector<GameObject*>& line : plane){
			line.resize(kMapSize.x);
		}
	}

	//ゲームオブジェクトの生成
	for (uint32_t y = 0; y < static_cast<uint32_t>(kMapSize.y); y++){
		for (uint32_t z = 0; z < static_cast<uint32_t>(kMapSize.z); z++){
			for (uint32_t x = 0; x < static_cast<uint32_t>(kMapSize.x); x++){
				//0の場合は抜ける
				if (map[y][z][x] == 0){
					continue;
				}
				GameObject*& block = mapBlocks[y][z][x];
				block = CreateGameObject();
				Object3d* object3d = block->AddComponent<Object3d>();
				object3d->SetModel(sceneContext_.modelManager->FindModel("cube"));
			}
		}
	}

	//位置を設定
	for (uint32_t y = 0; y < static_cast<uint32_t>(kMapSize.y); y++){
		for (uint32_t z = 0; z < static_cast<uint32_t>(kMapSize.z); z++){
			for (uint32_t x = 0; x < static_cast<uint32_t>(kMapSize.x); x++){
				//Nullなら抜ける
				if (!mapBlocks[y][z][x]){
					continue;
				}

				mapBlocks[y][z][x]->GetTransform().translate = {
					static_cast<float>(x) * kTileSize.x,
					static_cast<float>(y) * kTileSize.y,
					static_cast<float>(z) * kTileSize.z
				};
			}
		}
	}

	//プレイヤー
	GameObject* playerObject = CreateGameObject();
	Object3d* playerModel = playerObject->AddComponent<Object3d>();
	playerModel->SetModel(sceneContext_.modelManager->FindModel("sphere_32"));
	playerObject->AddComponent<Player>(*sceneContext_.input);
	playerObject->SetName("player");
	playerModel->SetColor(0, Vector4::MakeRedColor());
}

//デバッグ
void GameScene::Debug(){
	//基底クラスのデバッグ
	BaseScene::Debug();


}