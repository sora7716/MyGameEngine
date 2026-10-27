#include "SceneFactory.h"
#include "TestPlayScene.h"
#include "GameScene.h"

//デストラクタ
SceneFactory::~SceneFactory(){}

// シーンの生成
BaseScene* SceneFactory::CreateScene(const std::string& sceneName){
	//次のシーンの生成
	BaseScene* newScene = nullptr;
	if (sceneName == "TestPlay"){
		newScene = new TestPlayScene();
	} else if (sceneName == "Game"){
		newScene = new GameScene();
	}
	return newScene;
}

//コンストラクタ
SceneFactory::SceneFactory(AbstractSceneFactory::ConstructorKey key) :AbstractSceneFactory(key){}
