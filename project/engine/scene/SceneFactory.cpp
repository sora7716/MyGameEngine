#include "SceneFactory.h"
#include "TestPlayScene.h"

//デストラクタ
SceneFactory::~SceneFactory() {}

// シーンの生成
BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンの生成
	BaseScene* newScene = nullptr;
	//if (sceneName == "Title") {
	//	newScene = new TitleScene();
	//} else if (sceneName == "Game") {
	//	newScene = new GameScene();
	//} else if (sceneName == "Result") {
	//	newScene = new ResultScene();
	//} else if (sceneName == "GameOver") {
	//	newScene = new GameOverScene();
	//} else 
	if (sceneName == "TestPlay") {
		newScene = new TestPlayScene();
	}
	return newScene;
}

//コンストラクタ
SceneFactory::SceneFactory(AbstractSceneFactory::ConstructorKey key) :AbstractSceneFactory(key) {}
