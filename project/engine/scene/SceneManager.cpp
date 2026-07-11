#include "SceneManager.h"
#include "ImGuiManager.h"
#include <cassert>

//デストラクタ
SceneManager::~SceneManager() {
	scene_->Finalize();
	delete scene_;
	delete instance;
}

//初期化
void SceneManager::Initialize(const SceneContext& sceneContext) {
	sceneContext_ = sceneContext;
	//シーンマネージャだけ自分から渡す
	sceneContext_.sceneManager = this;
}

//更新
void SceneManager::Update() {
	//次のシーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}
		//シーンの切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		//次のシーン
		scene_->Initialize(sceneContext_);
	}
	//更新
	scene_->Update();
}

void SceneManager::Game() {
	scene_->SetGameCamera();
}

//デバッグ
void SceneManager::Debug() {
	scene_->SetDebugCamera();
#ifdef USE_IMGUI
	sceneContext_.imGuiManager->Begin();
	scene_->Debug();
	sceneContext_.imGuiManager->End();
#endif // USE_IMGUI
}

//描画
void SceneManager::Draw() {
	//描画
	scene_->Draw();
}

//シーンファクトリーのセッター
void SceneManager::SetSceneFactory(AbstractSceneFactory* sceneFactory) {
	sceneFactory_ = sceneFactory;
}

//シーンの切り替え
void SceneManager::ChangeScene(const std::string& sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

//コンストラクタ
SceneManager::SceneManager(ConstructorKey) {
}
