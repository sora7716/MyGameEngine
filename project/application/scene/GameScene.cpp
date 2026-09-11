#include "GameScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Player.h"
#include "SkyBox.h"
#include "OrbitCameraController.h"
#include "AABBCollider.h"
#include "Cube.h"
#include "ImGuiManager.h"

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
	playerHitBoxSize_ = Vector3::MakeAllOne();
	GameObject* playerObject = CreateGameObject();
	playerAABB_ = playerObject->AddComponent<AABBCollider>();
	playerHitBox_ = playerObject->AddComponent<debugDraw::Cube>();
	Object3d* playerModel = playerObject->AddComponent<Object3d>();
	playerModel->SetModel(sceneContext_.modelManager->FindModel("player"));
	Player* player = playerObject->AddComponent<Player>(*sceneContext_.input);
	playerObject->SetName("player");
	playerAABB_->SetBodyType(BodyType::kDynamic);


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

	//キューブオブジェクト
	GameObject* cubeObject = CreateGameObject();
	cubeObject->SetName("cube");
	cubeObject->GetTransform().translate = { 0.0f,1.0f,2.0f };
	Object3d* cubeModel = cubeObject->AddComponent<Object3d>();
	cubeModel->SetModel(sceneContext_.modelManager->FindModel("cube"));
	Vector3 cubeHitBoxSize = Vector3::MakeAllOne();
	AABBCollider* cubeAABB = cubeObject->AddComponent<AABBCollider>();
	debugDraw::Cube* cubeHitBox = cubeObject->AddComponent<debugDraw::Cube>();
	cubeAABB->SetHalfSize(cubeHitBoxSize / 2.0f);
	cubeHitBox->SetLocalScale(cubeHitBoxSize);
}

//更新のステート
void GameScene::UpdateState(){
	//サイズの設定
	playerAABB_->SetHalfSize(playerHitBoxSize_ / 2.0f);
	playerHitBox_->SetLocalScale(playerHitBoxSize_);
}

//デバッグ
void GameScene::Debug(){
	//基底クラスのデバッグ
	BaseScene::Debug();

#ifdef USE_IMGUI
	ImGui::Begin("デバッグ");
	ImGui::DragFloat3("aabb.size", &playerHitBoxSize_.x, 0.1f);
	ImGui::End();
#endif // USE_IMGUI
}

//解放処理
void GameScene::Finalize(){
	//基底クラスの解放
	BaseScene::Finalize();
}
