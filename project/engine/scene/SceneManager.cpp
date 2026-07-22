#include "SceneManager.h"
#include "ImGuiManager.h"
#include "debugEditor.h"
#include <cassert>

//コンストラクタ
SceneManager::SceneManager(){
}

//デストラクタ
SceneManager::~SceneManager(){
	scene_->Finalize();
	delete scene_;
	delete instance;
}

//初期化
void SceneManager::Initialize(const SceneContext& sceneContext){
	sceneContext_ = sceneContext;
	//シーンマネージャだけ自分から渡す
	sceneContext_.sceneManager = this;
	//デバッグエディターの生成と初期化
	debugEditor_ = std::make_unique<DebugEditor>();
	debugEditor_->Initialize(sceneContext_.tagManager);
}

//更新
void SceneManager::Update(){
	//次のシーンの予約があるなら
	if (nextScene_){
		//旧シーンの終了
		if (scene_){
			//デバッグエディタの初期化
			debugEditor_->Initialize(sceneContext_.tagManager);
			//旧シーンの解放
			scene_->Finalize();
			delete scene_;
		}
		//シーンの切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		//次のシーン
		scene_->Initialize(sceneContext_);
		//ゲームオブジェクト一覧をDebugEditorに登録
		debugEditor_->SetGameObjects(scene_->GetGameObjects());
	}
	//更新
	scene_->Update();
	//デバッグエディタの更新
	debugEditor_->Update();
}

//デバッグ
void SceneManager::Debug(){
#ifdef USE_IMGUI
	sceneContext_.imGuiManager->Begin();
	//デバッグエディタの描画
	debugEditor_->Draw();

	//生成要求
	if (debugEditor_->ConsumeCreateRequest()){
		GameObject* newGameObject = scene_->CreateGameObject();
		//生成したGameObjectを選択
		debugEditor_->SelectGameObject(newGameObject);
	}

	//複製要求
	if (GameObject* target = debugEditor_->ConsumeDuplicateRequest()){
		scene_->DuplicateGameObject(target);
	}

	//削除要求
	if (GameObject* target = debugEditor_->ConsumeDeleteRequest()){
		scene_->DeleteGameObject(target);
	}

	//ゲームオブジェクトの移動要求
	if (debugEditor_->ConsumeMoveGameObjectRequest());

	//シーンのデバッグ
	scene_->Debug();
	sceneContext_.imGuiManager->End();
#endif // USE_IMGUI
}

//ゲーム画面の描画
void SceneManager::GameDraw(){
	//描画
	scene_->GameDraw();
}

//デバッグ画面の描画
void SceneManager::DebugDraw(){
	//描画
	scene_->DebugDraw();
}

//シーンファクトリーのセッター
void SceneManager::SetSceneFactory(AbstractSceneFactory* sceneFactory){
	sceneFactory_ = sceneFactory;
}

//シーンの切り替え
void SceneManager::ChangeScene(const std::string& sceneName){
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

//コンストラクタ
SceneManager::SceneManager(ConstructorKey){
}
