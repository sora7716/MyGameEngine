#include "GameScene.h"
#include "GameObject.h"
#include "SkyBox.h"
#include "Object3d.h"
#include "DirectionalLight.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Player.h"
#include "MovementCommand.h"
#include "JumpCommand.h"
#include "StickMovementCommand.h"
#include "StickCameraControlCommand.h"
#include "PlacementPreviewCommand.h"
#include "DefenseRotationCommand.h"
#include "PlacementSwitchCommand.h"
#include "DefensePlaceCommand.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include "JumpComponent.h"
#include "CameraControlComponent.h"
#include "MapChipField.h"
#include "MapChipCollider.h"
#include "Camera.h"
#include "FollowCameraController.h"
#include "ImGuiManager.h"
#include "CreateFieldSystem.h"
#include "DefensePlacementSystem.h"
#include "ArrowSystem.h"
#include "Health.h"
#include "Enemy.h"
#include "HealthBar.h"
#include <array>
using namespace mapChipDesc;

//コンストラクタ
GameScene::GameScene() {
}

//デストラクタ
GameScene::~GameScene() {
}

//初期化
void GameScene::Initialize() {
	//基底クラスの初期化
	BaseScene::Initialize();

#ifdef _DEBUG
	//移動用コマンドの生成
	movementCommands_.resize(static_cast<uint32_t>(MovementCommandMode::kCount));
	for(std::unique_ptr<MovementCommand>& movementCommand : movementCommands_) {
		movementCommand = std::make_unique<MovementCommand>();
	}
	//前へ移動
	movementCommands_[static_cast<uint32_t>(MovementCommandMode::kMoveFowerd)]->Setup({0.0f,0.0f,1.0f});
	//後ろへ移動
	movementCommands_[static_cast<uint32_t>(MovementCommandMode::kMoveBack)]->Setup({0.0f,0.0f,-1.0f});
	//右へ移動
	movementCommands_[static_cast<uint32_t>(MovementCommandMode::kMoveRight)]->Setup({1.0f,0.0f,0.0f});
	//左へ移動
	movementCommands_[static_cast<uint32_t>(MovementCommandMode::kMoveLeft)]->Setup({-1.0f,0.0f,0.0f});
#endif // _DEBUG

	//スティックの移動用コマンドを生成
	jumpCommand_ = std::make_unique<JumpCommand>();

	//カメラの生成
	cameraObject_ = CreateCamera();

	//平行光源の生成
	CreateDirectionalLight();

	//SkyBoxの生成
	CreateSkyBox();

	//フィールド生成システムの生成
	createFieldSystem_ = CreateFieldSystem::Create(this,"map_04.csv");

	//プレイヤーのゲームオブジェクトを生成
	playerObject_ = CreateGameObject();

	//矢の管理すステムの生成
	arrowSystem_ = ArrowSystem::Create(this);
	//矢の攻撃力を設定
	arrowSystem_->SetAttackPower(20.0f);

	//防衛オブジェクトの設置システムの生成
	defensePlacementSystem_ = DefensePlacementSystem::Create(this,createFieldSystem_->GetMapChipField(),playerObject_,arrowSystem_.get());

	//プレイヤーの作成
	CreatePlayer();
	//カメラのターゲットを追加
	followCamera_->SetTarget(playerObject_);

	//敵のHPバーのマテリアルを作成
	MaterialTexturePaths enemyHpBarMatTexPaths = {
		.textureFilePath = "textures/white1x1.png",
		.environmentMap = "textures/skybox_cube.dds"
	};
	AddMaterialInstance("enemyHpBarMat",enemyHpBarMatTexPaths);
	MaterialInstance* enemyHpBarMat = FindMaterialInstance("enemyHpBarMat");
	enemyHpBarMat->SetParameter("color",Color::GetRed());
	enemyHpBarMat->SetParameter("enabledLighting",uint32_t{0});

	//敵のHPバーの背景のマテリアル
	AddMaterialInstance("enemyHpBarBgMat",enemyHpBarMatTexPaths);
	MaterialInstance* enemyHpBarBgMat = FindMaterialInstance("enemyHpBarBgMat");
	enemyHpBarBgMat->SetParameter("color",Color::GetBlack());
	enemyHpBarBgMat->SetParameter("enabledLighting",uint32_t{0});

	//敵の生成
	enemyObjects_.push_back(CreateEnemy({0.0f,2.0f,0.0f}));
	enemyObjects_.push_back(CreateEnemy({1.0f,2.0f,0.0f}));
	enemyObjects_.push_back(CreateEnemy({2.0f,2.0f,0.0f}));
}

//更新のステート
void GameScene::UpdateState() {
	//防衛オブジェクトの設置システムの更新
	defensePlacementSystem_->Update();
	//敵の設定
	defensePlacementSystem_->SetEnemies(enemyObjects_);
}

//デバッグ
void GameScene::Debug() {
#ifdef USE_IMGUI
	//基底クラスのデバッグ
	BaseScene::Debug();
	ImGui::Begin("マップチップに関する物");
	ImGui::End();
#endif // USE_IMGUI
}

//解放処理
void GameScene::Finalize() {
	//基底クラスの解放
	BaseScene::Finalize();
}

//平行光源の生成
void GameScene::CreateDirectionalLight() {
	//平行光源
	GameObject* gameObject = CreateGameObject();
	gameObject->AddComponent<DirectionalLight>();
	gameObject->GetTransform().SetEulerAngleDegrees({-120.0f,0.0f,0.0f});
	gameObject->SetName("平行光源");
}

//カメラの生成
GameObject* GameScene::CreateCamera() {
	//カメラ
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("ゲームカメラ");
	gameObject->AddComponent<Camera>();
	followCamera_ = gameObject->AddComponent<FollowCameraController>();
	followCamera_->SetRadius(9.0f);
	followCamera_->SetTheta(0.5f);

	//カメラ操作用の機能を追加
	cameraControlComponent_ = gameObject->AddComponent<CameraControlComponent>();
	//カメラさ操作用の機能のセットアップ
	float followSpeed = 10.0f;//追従カメラの補間速度
	cameraControlComponent_->Setup(followSpeed);
	return gameObject;
}

//SkyBoxの生成
GameObject* GameScene::CreateSkyBox() {
	//ゲームオブジェクト
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().scale = {100.0f,100.0f,100.0f};
	gameObject->SetName("SkyBox");
	gameObject->SetTag("SkyBox");

	//SkyBox
	gameObject->AddComponent<SkyBox>();

	return gameObject;
}

//プレイヤーの作成
void GameScene::CreatePlayer() {
	//コマンドの生成
	//カメラ操作用のコマンド
	std::unique_ptr<StickCameraControlCommand>stickCameraControlCommand = std::make_unique<StickCameraControlCommand>();
	//スティックの移動用コマンド
	std::unique_ptr<StickMovementCommand>stickMoveCommand = std::make_unique<StickMovementCommand>();;
	//オブジェクトの設置予定地の表示/非表示のコマンド
	std::unique_ptr<PlacementPreviewCommand>placementPreviewCommand = std::make_unique<PlacementPreviewCommand>();
	//オブジェクトの回転用コマンド
	std::vector<std::unique_ptr<DefenseRotationCommand>>defenseRotationCommands(static_cast<uint32_t>(DirectionType::kCount));
	for(std::unique_ptr<DefenseRotationCommand>& defenseRotationCommand : defenseRotationCommands) {
		defenseRotationCommand = std::make_unique<DefenseRotationCommand>();
	}
	//左回転
	defenseRotationCommands[static_cast<uint32_t>(DirectionType::kLeft)]->Setup(-1.0f);
	//右回転
	defenseRotationCommands[static_cast<uint32_t>(DirectionType::kRight)]->Setup(1.0f);
	//設置するオブジェクトの切り替え用コマンド
	std::vector<std::unique_ptr<PlacementSwitchCommand>>placementSwitchCommands(static_cast<uint32_t>(DirectionType::kCount));
	for(std::unique_ptr<PlacementSwitchCommand>& placementSwitchCommand : placementSwitchCommands) {
		placementSwitchCommand = std::make_unique<PlacementSwitchCommand>();
	}
	//左へ
	placementSwitchCommands[static_cast<uint32_t>(DirectionType::kLeft)]->Setup(-1);
	//右へ
	placementSwitchCommands[static_cast<uint32_t>(DirectionType::kRight)]->Setup(1);
	//オブジェクトを設置する用のコマンド
	std::unique_ptr<DefensePlaceCommand> defensePlaceCommand = std::make_unique<DefensePlaceCommand>();

	//ゲームオブジェクトを生成
	GameObject* gameObject = playerObject_;
	Vector3Int playerPosIndex = createFieldSystem_->GetMapChipField()->GetMapChipData().playerIndex;
	gameObject->GetTransform().translate = createFieldSystem_->GetMapChipField()->GetMapChipPositionByIndex(playerPosIndex);
	gameObject->SetName("プレイヤー");
	gameObject->SetTag("Player");
	//オブジェクト3dを追加
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("player");

	//インプット機能を追加
	InputComponent* inputComponent = gameObject->AddComponent<InputComponent>();
	inputComponent->Setup(GetSceneContext().input);

	//移動用の機能を追加
	MovementComponent* movementComponent = gameObject->AddComponent<MovementComponent>();
	//移動用の機能のセットアップ
	float moveSpeed = 7.5f;//移動速度
	float moveResponse = 6.0f;//移動した際のレスポンス
	movementComponent->Setup(moveSpeed,moveResponse);
	movementComponent->SetCameraObject(cameraObject_);

	//ジャンプ用の機能を追加
	JumpComponent* jumpComponent = gameObject->AddComponent<JumpComponent>();
	//ジャンプ用の機能のセットアップ
	float jumpSpeed = 7.0f;
	jumpComponent->Setup(jumpSpeed);

	//リジッドボディを追加
	RigidBody* rigidBody = gameObject->AddComponent<RigidBody>();
	movementComponent->SetRigidBody(rigidBody);
	jumpComponent->SetRigidBody(rigidBody);

	//スティックの移動コマンドを追加	
	stickMoveCommand->SetMovementComponent(movementComponent);
	inputComponent->AddAxisInput(StickType::kLeft,std::move(stickMoveCommand->Clone()));

	//スティックのカメラ操作用のコマンドを追加
	stickCameraControlCommand->SetCameraControlComponent(cameraControlComponent_);
	inputComponent->AddAxisInput(StickType::kRight,std::move(stickCameraControlCommand->Clone()));

	//ジャンプコマンドを追加
	jumpCommand_->SetJumpComponent(jumpComponent);
	//ジャンプコマンドを追加
	inputComponent->AddInputState(XboxInputType::kA,InputPhase::kPressed,std::move(jumpCommand_->Clone()));

	//設置予定地の表示/非表示コマンドを追加
	placementPreviewCommand->SetPlacementPreviewComponent(defensePlacementSystem_->GetPlacementPreview());
	//設置予定地の表示/非表示コマンドを追加
	inputComponent->AddInputState(XboxInputType::kB,InputPhase::kPressed,std::move(placementPreviewCommand->Clone()));

	//設置オブジェクトの回転
	for(uint32_t i = 0; i < static_cast<uint32_t>(DirectionType::kCount); i++) {
		//オブジェクト回転用機能を追加
		defenseRotationCommands[i]->SetDefenseRotationComponent(defensePlacementSystem_->GetDefenseRotation());
	}
	//左回転
	inputComponent->AddInputState(XboxInputType::kLB,InputPhase::kPressed,std::move(defenseRotationCommands[static_cast<uint32_t>(DirectionType::kLeft)]->Clone()));
	//右回転
	inputComponent->AddInputState(XboxInputType::kRB,InputPhase::kPressed,std::move(defenseRotationCommands[static_cast<uint32_t>(DirectionType::kRight)]->Clone()));

	//設置オブジェクトの切り替え
	for(uint32_t i = 0; i < static_cast<uint32_t>(DirectionType::kCount); i++) {
		//設置オブジェクトの切り替えを追加
		placementSwitchCommands[i]->SetPlacementSwitchComponent(defensePlacementSystem_->GetPlacementSwitch());
	}
	//左回転
	inputComponent->AddInputState(XboxInputType::kDPadLeft,InputPhase::kPressed,std::move(placementSwitchCommands[static_cast<uint32_t>(DirectionType::kLeft)]->Clone()));
	//右回転
	inputComponent->AddInputState(XboxInputType::kDPadRight,InputPhase::kPressed,std::move(placementSwitchCommands[static_cast<uint32_t>(DirectionType::kRight)]->Clone()));

	//オブジェクトの設置
	defensePlaceCommand->SetDefensePlaceComponent(defensePlacementSystem_->GetDefensePlace());
	inputComponent->AddInputState(XboxInputType::kRT,InputPhase::kPressed,std::move(defensePlaceCommand->Clone()));

#ifdef _DEBUG
	//キーの一覧
	std::array<const char,static_cast<uint32_t>(MovementCommandMode::kCount)>moveKeyList{
		DIK_W,
		DIK_S,
		DIK_A,
		DIK_D
	};
	//移動を追加
	for(uint32_t i = 0; i < moveKeyList.size(); i++) {
		//入力キーを取得
		char key = moveKeyList[i];
		//移動用機能を追加
		movementCommands_[i]->SetMovementComponent(movementComponent);
		//移動コマンドを追加
		inputComponent->AddInputState(static_cast<BYTE>(key),InputPhase::kHeld,std::move(movementCommands_[i]->Clone()));
	}
	inputComponent->AddInputState(static_cast<BYTE>(DIK_SPACE),InputPhase::kPressed,std::move(jumpCommand_->Clone()));
#endif // _DEBUG

	//プレイヤーの設定
	Player* player = gameObject->AddComponent<Player>();
	player->SetObject3d(object3d);
	player->SetInput(GetSceneContext().input);

	//マップチップのコライダーを追加
	MapChipCollider* mapChipCollider = gameObject->AddComponent<MapChipCollider>();
	//マップチップコライダーのセットアップ
	Vector3 hitBoxSize = {0.8f,0.8f,0.8f};
	mapChipCollider->Setup(createFieldSystem_->GetMapChipField(),hitBoxSize);

	//Boxコライダーを追加
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetSize(hitBoxSize);
	boxCollider->SetBodyType(BodyType::kDynamic);
}

//敵の生成
GameObject* GameScene::CreateEnemy(const Vector3& position) {
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().translate = position;
	gameObject->SetName("敵");
	gameObject->SetTag("Enemy");
	//Object3d
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("enemy");

	//Enemyを追加
	gameObject->AddComponent<Enemy>();

	//リジッドボディを追加
	gameObject->AddComponent<RigidBody>();

	//HPを設定
	Health* health = gameObject->AddComponent<Health>();
	health->SetMaxHp(100.0f);

	//マップチップのコライダーを追加
	MapChipCollider* mapChipCollider = gameObject->AddComponent<MapChipCollider>();
	//マップチップコライダーのセットアップ
	Vector3 hitBoxSize = {0.8f,0.8f,0.8f};
	mapChipCollider->Setup(createFieldSystem_->GetMapChipField(),hitBoxSize);

	//BoxColliderの追加
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetSize(hitBoxSize);
	boxCollider->SetBodyType(BodyType::kDynamic);

	//HPバーの子オブジェクトを作成
	GameObject* healthObject = CreateGameObject();
	healthObject->SetName("HPバー");
	//親子付け
	healthObject->AttachTo(gameObject,false);

	//Object3dの追加
	Object3d* healthBarObject3d = healthObject->AddComponent<Object3d>();
	healthBarObject3d->SetModel("healthBar");
	healthBarObject3d->SetRenderTransformMode(RenderTransformMode::kBillboard);
	healthBarObject3d->SetMaterialInstance(0,GetMaterialInstance("enemyHpBarMat"));

	//HealthBarを追加
	HealthBar* healthBar = healthObject->AddComponent<HealthBar>();
	healthBar->Setup(health);

	//HPが満タンのときの半分の横幅を取得
	constexpr float kBarMeshWidth = 0.5f;
	const float halfFullWidth = kBarMeshWidth * healthBar->GetFullBarScale() / 2.0f;
	healthBarObject3d->SetBillboardHorizontalOffset(-halfFullWidth);

	//HealthBarの後ろ背景
	GameObject* healthBarBgObject = CreateGameObject();
	//Transformの調整
	Transform& bgTransform = healthBarBgObject->GetTransform();
	const Transform& barTransform = healthObject->GetTransform();
	bgTransform.translate = barTransform.translate;
	bgTransform.scale.x = healthBar->GetFullBarScale();
	bgTransform.scale.y = barTransform.scale.y;
	healthBarBgObject->SetName("HPバーの背景");

	//親子付け
	healthBarBgObject->AttachTo(gameObject,false);

	//Object3dの追加
	Object3d* healthBarBgObject3d = healthBarBgObject->AddComponent<Object3d>();
	healthBarBgObject3d->SetModel("healthBar");
	healthBarBgObject3d->SetRenderTransformMode(RenderTransformMode::kBillboard);
	healthBarBgObject3d->SetMaterialInstance(0,GetMaterialInstance("enemyHpBarBgMat"));
	healthBarBgObject3d->SetBillboardDepthOffset(0.01f);
	healthBarBgObject3d->SetBillboardHorizontalOffset(-halfFullWidth);

	return gameObject;
}