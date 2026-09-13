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
#include "Enemy.h"

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

	//プレイヤー
	gameObject->AddComponent<Player>();

	//AABBコンポーネント
	Vector3 playerHitBoxSize = Vector3::GetAllOne();
	AABBCollider* playerAABB = gameObject->AddComponent<AABBCollider>();
	playerAABB->SetHalfSize(playerHitBoxSize / 2.0f);
	playerAABB->SetBodyType(BodyType::kDynamic);

	//ワイヤーフレーム
	debugDraw::Cube* playerHitBox = gameObject->AddComponent<debugDraw::Cube>();
	playerHitBox->SetLocalScale(playerHitBoxSize);
	playerHitBox->SetColor(Vector4::MakeRedColor());

	//3Dオブジェクト
	Object3d* playerModel = gameObject->AddComponent<Object3d>();
	playerModel->SetModel("player");

	return gameObject;
}

//地面の生成
GameObject* GameScene::CreateGround(){
	//ゲームオブジェくtp
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().scale = { 20.0f,1.0f,20.0f };
	gameObject->SetName("地面");
	gameObject->SetTag("Ground");

	//3Dオブジェクト1
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

	//Enemy
	gameObject->AddComponent<Enemy>();

	//AABBコンポーネント
	Vector3 enemyHitBoxSize = Vector3::GetAllOne();
	AABBCollider* enemyAABB = gameObject->AddComponent<AABBCollider>();
	enemyAABB->SetIsTrigger(true);

	//ワイヤーフレーム
	debugDraw::Cube* enemyHitBox = gameObject->AddComponent<debugDraw::Cube>();
	enemyAABB->SetHalfSize(enemyHitBoxSize / 2.0f);
	enemyHitBox->SetLocalScale(enemyHitBoxSize);

	//3Dオブジェクト
	Object3d* enemyModel = gameObject->AddComponent<Object3d>();
	enemyModel->SetModel("enemy");

	return gameObject;
}
