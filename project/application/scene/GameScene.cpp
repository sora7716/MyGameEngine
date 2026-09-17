#include "GameScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Player.h"
#include "SkyBox.h"
#include "OrbitCameraController.h"
#include "AABBCollider.h"
#include "Cube.h"
#include "ImGuiManager.h"
#include "Enemy.h"
#include "RigidBody.h"

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
	Object3d* playerObject3d = playerObject_->GetComponent<Object3d>();
	for (const Object3d::NodeInfo& nodeInfo : playerObject3d->GetNodeNames()){
		ImGui::Text("・%s", nodeInfo.path.c_str());
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
	gameObject->SetTag("SkyBox");

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
	gameCameraModel->SetModel("camera");

	//オービットカメラ
	OrbitCameraController* orbitCamera = gameObject->AddComponent<OrbitCameraController>();
	orbitCamera->SetTarget(playerObject);

	return gameObject;
}

//プレイヤーの生成
GameObject* GameScene::CreatePlayerObject(){
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("プレイヤー");
	gameObject->SetTag("Player");

	//3Dオブジェクト
	Object3d* playerModel = gameObject->AddComponent<Object3d>();
	playerModel->SetModel("player");

	//リジットボディ
	gameObject->AddComponent<RigidBody>();

	//プレイヤー
	gameObject->AddComponent<Player>();

	//AABBコンポーネント
	Vector3 playerHitBoxSize = Vector3::GetOne();
	AABBCollider* playerAABB = gameObject->AddComponent<AABBCollider>();
	playerAABB->SetHalfSize(playerHitBoxSize / 2.0f);
	playerAABB->SetBodyType(BodyType::kDynamic);

	//ワイヤーフレーム
	debugDraw::Cube* playerHitBox = gameObject->AddComponent<debugDraw::Cube>();
	playerHitBox->SetLocalScale(playerHitBoxSize);
	playerHitBox->SetColor(Vector4::GetRedColor());

	return gameObject;
}

//地面の生成
GameObject* GameScene::CreateGround(){
	//ゲームオブジェく
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().scale = { 20.0f,1.0f,20.0f };
	gameObject->SetName("地面");
	gameObject->SetTag("Ground");

	//AABBコライダー
	Vector3 blockerSize = Vector3::GetOne();
	AABBCollider* blocker = gameObject->AddComponent<AABBCollider>();
	blocker->SetHalfSize(blockerSize / 2.0f);
	blocker->SetBodyType(BodyType::kStatic);
	debugDraw::Cube* blockerDebug = gameObject->AddComponent<debugDraw::Cube>();
	blockerDebug->SetLocalScale(blockerSize);

	//3Dオブジェクト
	Object3d* groundModel = gameObject->AddComponent<Object3d>();
	groundModel->SetModel("cube");
	groundModel->SetTexture(0, "uvChecker.png");

	return gameObject;
}

//敵の生成
GameObject* GameScene::CreateEnemy(){
	//ゲームオブジェクト
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("敵");
	gameObject->SetTag("Enemy");
	gameObject->GetTransform().translate = { 0.0f,1.0f,2.0f };

	//リジッドボディ
	gameObject->AddComponent<RigidBody>();

	//Enemy
	gameObject->AddComponent<Enemy>();

	//AABBコンポーネント
	Vector3 hitBoxSize = Vector3::GetOne();
	AABBCollider* hitBox = gameObject->AddComponent<AABBCollider>();
	hitBox->SetIsTrigger(true);
	hitBox->SetBodyType(BodyType::kDynamic);

	//ワイヤーフレーム
	debugDraw::Cube* hitBoxDebug = gameObject->AddComponent<debugDraw::Cube>();
	hitBox->SetHalfSize(hitBoxSize / 2.0f);
	hitBoxDebug->SetLocalScale(hitBoxSize);

	//すり抜け防止用のコライダー
	Vector3 blockerSize = { 0.3f,0.3f,0.3f };
	Vector3 blockerOffset = { 0.0f,-0.4f,0.0f };
	AABBCollider* blocker = gameObject->AddComponent<AABBCollider>();
	blocker->SetHalfSize(blockerSize / 2.0f);
	blocker->SetOffset(blockerOffset);
	blocker->SetIsTrigger(false);
	blocker->SetBodyType(BodyType::kDynamic);

	//ワイヤーフレーム
	debugDraw::Cube* blockerDebug = gameObject->AddComponent<debugDraw::Cube>();
	blockerDebug->SetLocalScale(blockerSize);
	blockerDebug->SetLocalTranslate(blockerOffset);
	blockerDebug->SetColor(Vector4::GetRedColor());

	//3Dオブジェクト
	Object3d* enemyModel = gameObject->AddComponent<Object3d>();
	enemyModel->SetModel("enemy");

	return gameObject;
}
