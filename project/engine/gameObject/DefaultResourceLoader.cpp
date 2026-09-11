#include "DefaultResourceLoader.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include <string>
#include <cassert>

//生成
std::unique_ptr<DefaultResourceLoader> DefaultResourceLoader::Create(ConstructorKey key, ModelManager* modelManager, TextureManager* textureManager, AudioManager* audioManager){
	//生成
	std::unique_ptr<DefaultResourceLoader>instance = std::make_unique<DefaultResourceLoader>(key);
	//初期化
	instance->Initialize(modelManager, textureManager, audioManager);

	return instance;
}

//コンストラクタ
DefaultResourceLoader::DefaultResourceLoader(ConstructorKey){
}

//デストラクタ
DefaultResourceLoader::~DefaultResourceLoader(){
}

//初期化
void DefaultResourceLoader::Initialize(ModelManager* modelManager, TextureManager* textureManager, AudioManager* audioManager){
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
void DefaultResourceLoader::LoadAudio(){
	audioManager_->LoadAudio("Alarm01", "Alarm01");
	audioManager_->LoadAudio("mokugyo", "mokugyo");
}

//テクスチャの読み込み
void DefaultResourceLoader::LoadTexture(){
	std::string directoryPath = "engine/resources/textures/";
	textureManager_->AddTexture(directoryPath + "magenta1x1.png");
	textureManager_->AddTexture(directoryPath + "white1x1.png");
	textureManager_->AddTexture(directoryPath + "skybox_cube.dds");
	textureManager_->AddTexture(directoryPath + "circle2.png");
	textureManager_->AddTexture(directoryPath + "monsterBall.png");
	textureManager_->AddTexture(directoryPath + "uvChecker.png");
}

//OBJファイルの読み込み
void DefaultResourceLoader::LoadModel(){
	//プリミティブなモデルの生成
	modelManager_->CreatePrimitiveModel();
	//カメラ
	//core_->GetModelManager()->AddModel("camera", "camera/camera.obj");
	//プレイヤー
	modelManager_->AddModel("player", "player/player.gltf");
	//デカヌ
	//core_->GetModelManager()->AddModel("dekanu", "dekanu/dekanu.gltf");
	//人
	//core_->GetModelManager()->AddModel("sneakWalk", "human/sneakWalk.gltf");
}