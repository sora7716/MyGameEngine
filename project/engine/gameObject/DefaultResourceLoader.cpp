#include "DefaultResourceLoader.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "Model.h"
#include <string>
#include <cassert>
#include "PrimitiveMeshFactory.h"

//生成
std::unique_ptr<DefaultResourceLoader> DefaultResourceLoader::Create(ConstructorKey key,ModelManager* modelManager,TextureManager* textureManager,AudioManager* audioManager) {
	//生成
	std::unique_ptr<DefaultResourceLoader>instance = std::make_unique<DefaultResourceLoader>(key);
	//初期化
	instance->Initialize(modelManager,textureManager,audioManager);

	return instance;
}

//コンストラクタ
DefaultResourceLoader::DefaultResourceLoader(ConstructorKey) {
}

//デストラクタ
DefaultResourceLoader::~DefaultResourceLoader() {
}

//初期化
void DefaultResourceLoader::Initialize(ModelManager* modelManager,TextureManager* textureManager,AudioManager* audioManager) {
	//管理者の記録
	assert(modelManager);
	modelManager_ = modelManager;
	assert(textureManager);
	textureManager_ = textureManager;
	assert(audioManager);
	audioManager_ = audioManager;

	//テクスチャの読み込み
	LoadTexture();
	//オーディオの読み込み
	LoadAudio();
	//OBJファイルの読み込み
	LoadModel();
}

//オーディオの読み込み
void DefaultResourceLoader::LoadAudio() {
	audioManager_->LoadAudio("Alarm01","Alarm01");
	audioManager_->LoadAudio("mokugyo","mokugyo");
}

//テクスチャの読み込み
void DefaultResourceLoader::LoadTexture() {
	std::string directoryPath = "engine/resources/";
	textureManager_->AddTexture(directoryPath + "textures/magenta1x1.png");
	textureManager_->AddTexture(directoryPath + "textures/white1x1.png");
	textureManager_->AddTexture(directoryPath + "textures/skybox_cube.dds");
	textureManager_->AddTexture(directoryPath + "textures/circle2.png");
	textureManager_->AddTexture(directoryPath + "textures/monsterBall.png");
	textureManager_->AddTexture(directoryPath + "textures/uvChecker.png");

	//デバッグエディタのアイコン
	textureManager_->AddTexture(directoryPath + "editorIcons/scale.png");
	textureManager_->AddTexture(directoryPath + "editorIcons/rotate.png");
	textureManager_->AddTexture(directoryPath + "editorIcons/translate.png");
}

//OBJファイルの読み込み
void DefaultResourceLoader::LoadModel() {
	//プリミティブなモデルの生成
	modelManager_->CreatePrimitiveModel();
	//プレイヤー
	modelManager_->AddModel("player","player/player.gltf");
	//敵
	modelManager_->AddModel("enemy","enemy/enemy.gltf");
	//ブロック
	modelManager_->AddModel("block","block/block.gltf");
	modelManager_->AddModel("wall","wall/wall.gltf");
	modelManager_->AddModel("thunderTower","thunderTower/thunderTower.gltf");
	modelManager_->AddModel("freezeTower","freezeTower/freezeTower.gltf");
	modelManager_->AddModel("bowTower","bowTower/bowTower.gltf");
	modelManager_->AddModel("gameMine","gameMine/gameMine.gltf");
	//矢
	modelManager_->AddModel("arrow","arrow/arrow.gltf");
	//弓タワーの探索範囲
	primitiveMeshFactory::Desc sectorDesc = {};
	sectorDesc.sweepAngleDegrees = 75.0f * 2.0f;
	modelManager_->AddModel("scanRange",primitiveMeshFactory::CreateSector(sectorDesc));

	//LODモデルは読み込み時に作る
	for(const std::string& name : {"wall","thunderTower","freezeTower","bowTower","gameMine"}) {
		modelManager_->FindModel(name)->CreateLODModels({0.7f,0.5f,0.25f});
	}
}