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
	//カメラの生成
	CreateCamera();
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
	core_->GetTextureManager()->AddTexture(directoryPath + "rostock_laage_airport_4k.dds");
	core_->GetTextureManager()->AddTexture(directoryPath + "skybox_cube.dds");
	core_->GetTextureManager()->AddTexture(directoryPath + "circle2.png");
	core_->GetTextureManager()->AddTexture(directoryPath + "monsterBall.png");
	core_->GetTextureManager()->AddTexture(directoryPath + "uvChecker.png");
}

//OBJファイルの読み込み
void GameObjectList::LoadModel(){
	//プリミティブなモデルの生成
	core_->GetModelManager()->CreatePrimitiveModel();
	//デカヌ
	core_->GetModelManager()->AddModel("dekanu", "dekanu/dekanu.gltf");
	//人
	core_->GetModelManager()->AddModel("sneakWalk", "human/sneakWalk.gltf");
}

//カメラの生成
void GameObjectList::CreateCamera(){
	//カメラの管理
	core_->GetCameraManager()->CreateCamera("defaultCamera");
	//デバッグカメラ
	core_->GetCameraManager()->CreateCamera("debugCamera");
	//タイトルカメラ
	core_->GetCameraManager()->CreateCamera("titleCamera");
	//ゲームカメラ
	core_->GetCameraManager()->CreateCamera("gameCamera");
	//リザルトカメラ
	core_->GetCameraManager()->CreateCamera("resultCamera");
	//テストプレイカメラ
	core_->GetCameraManager()->CreateCamera("testPlayCamera");
}