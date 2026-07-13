#include "BaseScene.h"
#include "DebugCamera.h"
#include "AbstractSceneFactory.h"
#include "GlobalVariables.h"
#include "WinApi.h"
#include "GameObject.h"
//#include "algorithms/ColliderManager.h"

//コンストラクタ
BaseScene::BaseScene(){
}

//デストラクタ
BaseScene::~BaseScene(){
}

//初期化
void BaseScene::Initialize(const SceneContext& sceneContext){
	//ゲームエンジンの核
	sceneContext_ = sceneContext;
	//デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(sceneContext_.input, sceneContext_.cameraManager);
	//コライダーマネージャー
	//colliderManager_ = std::make_unique<ColliderManager>();
	////調整ファイルの読み込み
	//GlobalVariables::GetInstance()->LoadFiles();
}

//更新
void BaseScene::Update(){
	//デバックカメラ
	debugCamera_->Update();
	//コライダーマネージャー
	//colliderManager_->ProcessCollision();
}

//デバッグ
void BaseScene::Debug(){

}

//終了
void BaseScene::Finalize(){
	//シーンファクトリーの解放
	delete sceneFactory_;
	sceneFactory_ = nullptr;
}

//ゲームオブジェクトの取得
const std::vector<std::unique_ptr<GameObject>>& BaseScene::GetGameObjects()const{
	// TODO: return ステートメントをここに挿入します
	return gameObjects_;
}
