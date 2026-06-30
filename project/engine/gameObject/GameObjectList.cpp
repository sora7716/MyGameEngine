#include "GameObjectList.h"
#include "engine/base/Core.h"
#include <cassert>
//デストラクタ
GameObjectList::~GameObjectList() {
}

//初期化
void GameObjectList::Initialize(Core* core) {
	//エンジンの核を記録する
	core_ = core;
	//オーディオの読み込み
	LoadAudio();
	//OBJファイルの読み込み
	LoadModel();
	//カメラの生成
	CreateCamera();
}

//コンストラクタ
GameObjectList::GameObjectList(ConstructorKey) {
}

//オーディオの読み込み
void GameObjectList::LoadAudio() {
	core_->GetAudioManager()->LoadAudio("Alarm01", "Alarm01");
	core_->GetAudioManager()->LoadAudio("mokugyo", "mokugyo");
}

//OBJファイルの読み込み
void GameObjectList::LoadModel() {
	//プリミティブなモデルの生成
	core_->GetModelManager()->CreatePrimitiveModel();
	////モデルの読み込み
	////デバッグ用
	////球
	//core_->GetModelManager()->LoadModel("sphere", "sphere", "sphere.gltf");
	////キューブ
	//core_->GetModelManager()->LoadModel("cube2", "cube", "cube.obj");
	//マルチメッシュ
	//core_->GetModelManager()->LoadModel("multiMaterial", "base", "multiMaterial.obj");

	////ゲームで使用するモデル
	////プレイヤー
	//core_->GetModelManager()->LoadModel("player", "player", "player.gltf");
	////敵
	//core_->GetModelManager()->LoadModel("enemy", "enemy", "enemy.gltf");
	//球
	core_->GetModelManager()->LoadModel("sphere", "sphere", "sphere.gltf");
	////弾
	//core_->GetModelManager()->LoadModel("bullet", "sphere", "sphere.gltf");
	////カメラ
	//core_->GetModelManager()->LoadModel("camera", "cube", "cube.obj");
	////地面
	//core_->GetModelManager()->LoadModel("ground", "ground", "ground.gltf");
	////フィールド
	//core_->GetModelManager()->LoadModel("field", "terrain", "terrain.obj");
	////平面
	//core_->GetModelManager()->LoadModel("plane", "base", "plane.gltf");
	////HPバー
	//core_->GetModelManager()->LoadModel("hpBar", "base", "plane.gltf");
	////HPアウトライン
	//core_->GetModelManager()->LoadModel("hpOutLine", "base", "plane.gltf");
	////スポットライト
	//core_->GetModelManager()->LoadModel("spotLight", "plane", "plane.obj");
	////軸
	//core_->GetModelManager()->LoadModel("test", "base", "axis.obj");
	////壁
	//core_->GetModelManager()->LoadModel("wall", "cube", "cube.obj");

	////高
	//core_->GetModelManager()->LoadModel("height", "height", "height.obj");
	////中
	//core_->GetModelManager()->LoadModel("medium", "medium", "medium.obj");
	////低
	//core_->GetModelManager()->LoadModel("low", "low", "low.obj");

	//デカヌ
	core_->GetModelManager()->LoadModel("dekanu", "dekanu", "dekanu.gltf");
	
	//杖
	//core_->GetModelManager()->LoadModel("staff", "staff", "staff.obj");
}

//カメラの生成
void GameObjectList::CreateCamera() {
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