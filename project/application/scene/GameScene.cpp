#include "GameScene.h"
#include "WinApi.h"
#include "GameObject.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Player.h"
#include "SkyBox.h"
#include "OrbitCameraController.h"

//コンストラクタ
GameScene::GameScene(){
}

//デストラクタ
GameScene::~GameScene(){
}

//初期化
void GameScene::Initialize(){
	//基底クラスの初期化
	BaseScene::Initialize();
	//SkyBox
	GameObject* skyBoxObject = CreateGameObject();
	skyBoxObject->AddComponent<SkyBox>();
	skyBoxObject->GetTransform().scale = { 100.0f,100.0f,100.0f };
	skyBoxObject->SetName("SkyBox");

	//プレイヤー
	GameObject* playerObject = CreateGameObject();
	Object3d* playerModel = playerObject->AddComponent<Object3d>();
	playerModel->SetModel(sceneContext_.modelManager->FindModel("player"));
	Player* player = playerObject->AddComponent<Player>(*sceneContext_.input);
	playerObject->SetName("player");
	//playerModel->SetColor(0, Vector4::MakeRedColor());

	//ゲームカメラの設定
	GameObject* gameCameraObject = CreateGameObject();
	gameCameraObject->SetName("ゲームカメラ");
	gameCameraObject->AddComponent<Camera>();
	Object3d* gameCameraModel = gameCameraObject->AddComponent<Object3d>();
	gameCameraModel->SetModel(sceneContext_.modelManager->FindModel("camera"));
	OrbitCameraController* orbitCamera = gameCameraObject->AddComponent<OrbitCameraController>(*sceneContext_.input);
	//オービットカメラの設定
	orbitCamera->SetTarget(playerObject);

	//プレイヤーにゲームカメラを設定
	player->SetCameraObject(gameCameraObject);

	//地面
	GameObject* groundObject = CreateGameObject();
	Object3d* groundModel = groundObject->AddComponent<Object3d>();
	groundModel->SetModel(sceneContext_.modelManager->FindModel("cube"));
	groundObject->GetTransform().scale = { 20.0f,1.0f,20.0f };
	groundModel->SetTexture(0, "uvChecker.png");;
	groundObject->SetName("地面");
}

//更新のステート
void GameScene::UpdateState(){
	sceneContext_.winApi->SetShowCursor(false);
}

//デバッグ
void GameScene::Debug(){
	//基底クラスのデバッグ
	BaseScene::Debug();
}