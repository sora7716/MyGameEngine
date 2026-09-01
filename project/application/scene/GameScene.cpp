#include "GameScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "ModelManager.h"

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
			{{1, 0, 0, 0, 0, 1}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0, 0}}
		}},
		{{
			{{1, 0, 0, 0, 0, 1}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}}
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
}

//更新
void GameScene::Update(){
	//基底クラスの更新
	BaseScene::Update();
}

//デバッグ
void GameScene::Debug(){
}

//終了
void GameScene::Finalize(){
}
