#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "SceneManager.h"
#include "Text.h"
#include "Core.h"
#include "Object3d.h"
#include "algorithms/Math.h"
#include "algorithms/ColliderManager.h"
#include "BaseShape.h"
#include "Line.h"
#include "Cube.h"
#include "Circle.h"
#include "Sphere.h"
#include <string>

//コンストラクタ
TestPlayScene::TestPlayScene() {};

//デストラクタ
TestPlayScene::~TestPlayScene() {};

//初期化
void TestPlayScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	BaseScene::Initialize(sceneContext);
	camera_ = sceneContext_.cameraManager->FindCamera("testPlayCamera");
}

//更新
void TestPlayScene::Update() {
	//シーンのインタフェースの初期化
	BaseScene::Update();
}

//デバッグ
void TestPlayScene::Debug() {
#ifdef USE_IMGUI

#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = sceneContext_.cameraManager->FindCamera("testPlayCamera");
	}
#endif // _DEBUG
}

//描画
void TestPlayScene::Draw() {
	
}

//終了
void TestPlayScene::Finalize() {
	//シーンのインターフェースの終了
	BaseScene::Finalize();
}
