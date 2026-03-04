#include "SceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"
#include "TestPlayScene.h"
//デストラクタ
SceneFactory::~SceneFactory() {
}

// シーンの生成
IScene* SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンの生成
	IScene* newScene = nullptr;
	if (sceneName == "Title") {
		newScene = new TitleScene();
	} else if (sceneName == "Game") {
		newScene = new GameScene();
	} else if (sceneName == "Result") {
		newScene = new ResultScene();
	} else if (sceneName == "TestPlay") {
		newScene = new TestPlayScene();
	}
	return newScene;
}

//コンストラクタ
SceneFactory::SceneFactory(AbstractSceneFactory::ConstructorKey key) :AbstractSceneFactory(key) {
}
