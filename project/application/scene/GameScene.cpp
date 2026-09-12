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

	//SkyBoxの生成
	CreateSkyBox();

	//プレイヤーの生成
	playerObject_ = CreatePlayerObject();

	//ゲームカメラの生成
	gameCameraObject_ = CreateGameCamera(playerObject_);

	//プレイヤーにカメラを設定
	playerObject_->GetComponent<Player>()->SetCameraObject(gameCameraObject_);

	//地面の生成
	CreateGround();

	//敵の生成
	CreateEnemy();
}

//更新のステート
void GameScene::UpdateState(){
}

//デバッグ
void GameScene::Debug(){
	//基底クラスのデバッグ
	BaseScene::Debug();

#ifdef USE_IMGUI
	ImGui::Begin("デバッグ");
	Vector3 playerHitBoxSize = playerObject_->GetComponent<AABBCollider>()->GetHalfSize() * 2.0f;
	if (ImGui::DragFloat3("aabb.size", &playerHitBoxSize.x, 0.1f)){
		playerObject_->GetComponent<AABBCollider>()->SetHalfSize(playerHitBoxSize / 2.0f);
		playerObject_->GetComponent<debugDraw::Cube>()->SetLocalScale(playerHitBoxSize);
	}
	ImGui::End();
#endif // USE_IMGUI
}

//解放処理
void GameScene::Finalize(){
	//基底クラスの解放
	BaseScene::Finalize();
}

//SkyBoxの生成
GameObject* GameScene::CreateSkyBox(){
	//ゲームオブジェクト
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().scale = { 100.0f,100.0f,100.0f };
	gameObject->SetName("SkyBox");

	//SkyBox
	gameObject->AddComponent<SkyBox>();

	return gameObject;
}

//ゲームカメラの生成
GameObject* GameScene::CreateGameCamera(GameObject* playerObject){
	//ゲームオブジェクト
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("ゲームカメラ");

	//カメラ
	gameObject->AddComponent<Camera>();

	//3Dオブジェクト
	Object3d* gameCameraModel = gameObject->AddComponent<Object3d>();
	gameCameraModel->SetModel(sceneContext_.modelManager->FindModel("camera"));

	//オービットカメラ
	OrbitCameraController* orbitCamera = gameObject->AddComponent<OrbitCameraController>(*sceneContext_.input);
	orbitCamera->SetTarget(playerObject);

	return gameObject;
}

//プレイヤーの生成
GameObject* GameScene::CreatePlayerObject(){
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("player");

	//プレイヤー
	gameObject->AddComponent<Player>(*sceneContext_.input);

	//AABBコンポーネント
	Vector3 playerHitBoxSize = Vector3::MakeAllOne();
	AABBCollider* playerAABB = gameObject->AddComponent<AABBCollider>();
	playerAABB->SetHalfSize(playerHitBoxSize / 2.0f);
	playerAABB->SetBodyType(BodyType::kDynamic);

	//ワイヤーフレーム
	debugDraw::Cube* playerHitBox = gameObject->AddComponent<debugDraw::Cube>();
	playerHitBox->SetLocalScale(playerHitBoxSize);
	playerHitBox->SetColor(Vector4::MakeRedColor());

	//3Dオブジェクト
	Object3d* playerModel = gameObject->AddComponent<Object3d>();
	playerModel->SetModel(sceneContext_.modelManager->FindModel("player"));

	return gameObject;
}

//地面の生成
GameObject* GameScene::CreateGround(){
	//ゲームオブジェくtp
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().scale = { 20.0f,1.0f,20.0f };
	gameObject->SetName("地面");

	//3Dオブジェクト1
	Object3d* groundModel = gameObject->AddComponent<Object3d>();
	groundModel->SetModel(sceneContext_.modelManager->FindModel("cube"));
	groundModel->SetTexture(0, "uvChecker.png");

	return gameObject;
}

//敵の生成
GameObject* GameScene::CreateEnemy(){
	//ゲームオブジェクト
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("cube");
	gameObject->GetTransform().translate = { 0.0f,1.0f,2.0f };

	//3Dオブジェクト
	Object3d* cubeModel = gameObject->AddComponent<Object3d>();
	cubeModel->SetModel(sceneContext_.modelManager->FindModel("cube"));

	//AABBコンポーネント
	Vector3 cubeHitBoxSize = Vector3::MakeAllOne();
	AABBCollider* cubeAABB = gameObject->AddComponent<AABBCollider>();

	//ワイヤーフレーム
	debugDraw::Cube* cubeHitBox = gameObject->AddComponent<debugDraw::Cube>();
	cubeAABB->SetHalfSize(cubeHitBoxSize / 2.0f);
	cubeHitBox->SetLocalScale(cubeHitBoxSize);

	return gameObject;
}
