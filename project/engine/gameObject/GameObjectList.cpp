#include "GameObjectList.h"
#include "Core.h"
#include <string>
#include <cassert>

//デストラクタ
GameObjectList::~GameObjectList(){
}

//初期化
void GameObjectList::Initialize(Core* core){
	//エンジンの核を記録する
	core_ = core;
	//テクスチャの読み込み
	LoadTexture();
	//オーディオの読み込み
	LoadAudio();
	//OBJファイルの読み込み
	LoadModel();
}

//コンストラクタ
GameObjectList::GameObjectList(ConstructorKey){
}

//オーディオの読み込み
void GameObjectList::LoadAudio(){
	core_->GetAudioManager()->LoadAudio("Alarm01", "Alarm01");
	core_->GetAudioManager()->LoadAudio("mokugyo", "mokugyo");
}

//テクスチャの読み込み
void GameObjectList::LoadTexture(){
	std::string directoryPath = "engine/resources/textures/";
	core_->GetTextureManager()->AddTexture(directoryPath + "magenta1x1.png");
	core_->GetTextureManager()->AddTexture(directoryPath + "white1x1.png");
	core_->GetTextureManager()->AddTexture(directoryPath + "skybox_cube.dds");
	core_->GetTextureManager()->AddTexture(directoryPath + "circle2.png");
	core_->GetTextureManager()->AddTexture(directoryPath + "monsterBall.png");
	core_->GetTextureManager()->AddTexture(directoryPath + "uvChecker.png");
}

//OBJファイルの読み込み
void GameObjectList::LoadModel(){
	//プリミティブなモデルの生成
	core_->GetModelManager()->CreatePrimitiveModel();
	//カメラ
	//core_->GetModelManager()->AddModel("camera", "camera/camera.obj");
	//プレイヤー
	core_->GetModelManager()->AddModel("player", "player/player.gltf");
	//デカヌ
	//core_->GetModelManager()->AddModel("dekanu", "dekanu/dekanu.gltf");
	//人
	//core_->GetModelManager()->AddModel("sneakWalk", "human/sneakWalk.gltf");
}