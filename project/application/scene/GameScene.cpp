#include "GameScene.h"
#include "GameObject.h"
#include "SkyBox.h"
#include "Object3d.h"
#include "Model.h"
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
#include "MathUtility.h"
#include "DefensePlacementSystem.h"
#include "Color.h"
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

	//ジャンプコマンドを生成
	stickMoveCommand_ = std::make_unique<StickMovementCommand>();

	//スティックの移動用コマンドを生成
	jumpCommand_ = std::make_unique<JumpCommand>();

	//カメラコントロール用のコマンド
	stickCameraControlCommand_ = std::make_unique<StickCameraControlCommand>();

	//設置予定地を1表示する用のコマンド
	placementPreviewCommand_ = std::make_unique<PlacementPreviewCommand>();

	//ディフェンス用のオブジェクトの回転コマンドの生成
	defenseRotationCommands_.resize(static_cast<uint32_t>(DirectionType::kCount));
	for(std::unique_ptr<DefenseRotationCommand>& defenseRotationCommand : defenseRotationCommands_) {
		defenseRotationCommand = std::make_unique<DefenseRotationCommand>();
	}
	//左回転
	defenseRotationCommands_[static_cast<uint32_t>(DirectionType::kLeft)]->Setup(-1.0f);
	//右回転
	defenseRotationCommands_[static_cast<uint32_t>(DirectionType::kRight)]->Setup(1.0f);

	//設置するオブジェクトの切り替え用コマンドの生成
	placementSwitchCommands_.resize(static_cast<uint32_t>(DirectionType::kCount));
	for(std::unique_ptr<PlacementSwitchCommand>& placementSwitchCommand : placementSwitchCommands_) {
		placementSwitchCommand = std::make_unique<PlacementSwitchCommand>();
	}
	//左へ
	placementSwitchCommands_[static_cast<uint32_t>(DirectionType::kLeft)]->Setup(-1);
	//右へ
	placementSwitchCommands_[static_cast<uint32_t>(DirectionType::kRight)]->Setup(1);

	//オブジェクトを設置する用のコマンドを生成
	defensePlaceCommand_ = std::make_unique<DefensePlaceCommand>();

	//マップチップフィールドを生成
	mapChipField_ = MapChipField::Create(mapChipCsvName_);

	//カメラの生成
	cameraObject_ = CreateCamera();

	//平行光源の生成
	CreateDirectionalLight();

	//SkyBoxの生成
	CreateSkyBox();

	//フィールドの生成
	CreateField();

	//プレイヤーのゲームオブジェクトを生成
	playerObject_ = CreateGameObject();

	//防衛オブジェクトの設置システムの生成1
	defensePlacementSystem_ = DefensePlacementSystem::Create(this,mapChipField_.get(),playerObject_);

	//プレイヤーの作成
	CreatePlayer();
	//カメラのターゲットを設定
	followCamera_->SetTarget(playerObject_);

	//敵の生成
	CreateEnemy();
}

//更新のステート
void GameScene::UpdateState() {
	//防衛オブジェクトの設置システムの更新
	defensePlacementSystem_->Update();
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

//フィールドの生成
void GameScene::CreateField() {
	//マップチップのデータ
	MapChipData mapChipData = mapChipField_->GetMapChipData();

	//フィールド
	GameObject* field = CreateGameObject();
	field->SetName("フィールド");
	//フィールドブロックの配列サイズを設定
	fieldBlocks_.resize(mapChipData.data.size());
	//各ブロックを生成
	for(uint32_t z = 0; z < mapChipData.data.size(); z++) {
		//フィールドブロックの配列サイズを設定
		fieldBlocks_[z].resize(mapChipData.data[z].size());
		//ステータスのデータを取得
		std::vector<MapChipStatus>mapChipStatusData = mapChipData.data[z];
		for(uint32_t x = 0; x < mapChipStatusData.size(); x++) {
			//ステータス
			MapChipStatus status = mapChipStatusData[x];
			//フィールドブロックの配列サイズを設定
			fieldBlocks_[z][x].resize(status.height);
			for(uint32_t y = 0; y < status.height; y++) {
				//空白だった場合
				if(status.type == MapChipType::kBlank) {
					fieldBlocks_[z][x][y] = nullptr;
					continue;
				}

				//ゲームオブジェクトの生成
				GameObject* gameObject = CreateGameObject();
				gameObject->AttachTo(field);
				gameObject->SetName("Block" + std::to_string(z) + "-" + std::to_string(x));
				Vector3Int positionIndex = {
					static_cast<int32_t>(x),
					static_cast<int32_t>(y),
					static_cast<int32_t>(z)
				};
				gameObject->GetTransform().translate = mapChipField_->GetMapChipPositionByIndex(positionIndex);
				Object3d* object3d = gameObject->AddComponent<Object3d>();
				object3d->SetModel("block");

				fieldBlocks_[z][x][y] = gameObject;
			}
		}
	}
}

//プレイヤーの作成
void GameScene::CreatePlayer() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = playerObject_;
	Vector3Int playerPosIndex = mapChipField_->GetMapChipData().playerIndex;
	gameObject->GetTransform().translate = mapChipField_->GetMapChipPositionByIndex(playerPosIndex);
	gameObject->SetName("プレイヤー");
	//オブジェクト3dを設定
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("player");

#ifdef _DEBUG
	//ワイヤーフレーム
	Object3d* wireFrame = gameObject->AddComponent<Object3d>();
	wireFrame->SetModel("hitBox");
	wireFrame->SetFillMode(rasterizerMode::FillMode::kWireframe);
	wireFrame->SetMaterialParameter(0,"color",Color::GetRed());
#endif // _DEBUG

	//インプット機能を追加
	InputComponent* inputComponent = gameObject->AddComponent<InputComponent>();
	inputComponent->Setup(GetSceneContext().input);

	//移動用の機能を追加
	MovementComponent* movementComponent = gameObject->AddComponent<MovementComponent>();
	//ジャンプ用の機能を追加
	JumpComponent* jumpComponent = gameObject->AddComponent<JumpComponent>();

	//リジッドボディを設定
	RigidBody* rigidBody = gameObject->AddComponent<RigidBody>();
	//移動用の機能のセットアップ
	float moveSpeed = 7.5f;//移動速度
	float moveResponse = 6.0f;//移動した際のレスポンス
	movementComponent->Setup(rigidBody,moveSpeed,moveResponse);
	movementComponent->SetCameraObject(cameraObject_);
	//ジャンプ用の機能のセットアップ
	float jumpSpeed = 7.0f;
	jumpComponent->Setup(rigidBody,jumpSpeed);

	//スティックの移動コマンドを追加	
	stickMoveCommand_->SetMovementComponent(movementComponent);
	inputComponent->AddAxisInput(StickType::kLeft,std::move(stickMoveCommand_->Clone()));

	//スティックのカメラ操作用のコマンドを追加
	stickCameraControlCommand_->SetCameraControlComponent(cameraControlComponent_);
	inputComponent->AddAxisInput(StickType::kRight,std::move(stickCameraControlCommand_->Clone()));

	//ジャンプコマンドを設定
	jumpCommand_->SetJumpComponent(jumpComponent);
	//ジャンプコマンドを追加
	inputComponent->AddInputState(XboxInputType::kA,InputPhase::kPressed,std::move(jumpCommand_->Clone()));

	//設置予定地の表示/非表示コマンドを設定
	placementPreviewCommand_->SetPlacementPreviewComponent(defensePlacementSystem_->GetPlacementPreview());
	//設置予定地の表示/非表示コマンドを追加
	inputComponent->AddInputState(XboxInputType::kB,InputPhase::kPressed,std::move(placementPreviewCommand_->Clone()));

	//設置オブジェクトの回転
	for(uint32_t i = 0; i < static_cast<uint32_t>(DirectionType::kCount); i++) {
		//オブジェクト回転用機能を追加
		defenseRotationCommands_[i]->SetDefenseRotationComponent(defensePlacementSystem_->GetDefenseRotation());
	}
	//左回転
	inputComponent->AddInputState(XboxInputType::kLB,InputPhase::kPressed,std::move(defenseRotationCommands_[static_cast<uint32_t>(DirectionType::kLeft)]->Clone()));
	//右回転
	inputComponent->AddInputState(XboxInputType::kRB,InputPhase::kPressed,std::move(defenseRotationCommands_[static_cast<uint32_t>(DirectionType::kRight)]->Clone()));

	//設置オブジェクトの切り替え
	for(uint32_t i = 0; i < static_cast<uint32_t>(DirectionType::kCount); i++) {
		//設置オブジェクトの切り替えを設定
		placementSwitchCommands_[i]->SetPlacementSwitchComponent(defensePlacementSystem_->GetPlacementSwitch());
	}
	//左回転
	inputComponent->AddInputState(XboxInputType::kDPadLeft,InputPhase::kPressed,std::move(placementSwitchCommands_[static_cast<uint32_t>(DirectionType::kLeft)]->Clone()));
	//右回転
	inputComponent->AddInputState(XboxInputType::kDPadRight,InputPhase::kPressed,std::move(placementSwitchCommands_[static_cast<uint32_t>(DirectionType::kRight)]->Clone()));

	//オブジェクトの設置
	defensePlaceCommand_->SetDefensePlaceComponent(defensePlacementSystem_->GetDefensePlace());
	inputComponent->AddInputState(XboxInputType::kRT,InputPhase::kPressed,std::move(defensePlaceCommand_->Clone()));

#ifdef _DEBUG
	//キーの一覧
	std::array<const char,static_cast<uint32_t>(MovementCommandMode::kCount)>moveKeyList{
		DIK_W,
		DIK_S,
		DIK_A,
		DIK_D
	};
	//移動を設定
	for(uint32_t i = 0; i < moveKeyList.size(); i++) {
		//入力キーを取得
		char key = moveKeyList[i];
		//移動用機能を追加
		movementCommands_[i]->SetMovementComponent(movementComponent);
		//移動コマンドを設定
		inputComponent->AddInputState(static_cast<BYTE>(key),InputPhase::kHeld,std::move(movementCommands_[i]->Clone()));
	}
	inputComponent->AddInputState(static_cast<BYTE>(DIK_SPACE),InputPhase::kPressed,std::move(jumpCommand_->Clone()));
#endif // _DEBUG

	//プレイヤーの設定
	Player* player = gameObject->AddComponent<Player>();
	player->SetObject3d(object3d);
	player->SetInput(GetSceneContext().input);

	//マップチップのコライダーを設定
	MapChipCollider* mapChipCollider = gameObject->AddComponent<MapChipCollider>();
	//マップチップコライダーのセットアップ
	Vector3 hitBoxSize = {0.8f,0.8f,0.8f};
	mapChipCollider->Setup(mapChipField_.get(),rigidBody,hitBoxSize);

	//Boxコライダーを設定
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetHalfSize(hitBoxSize / 2.0f);
	boxCollider->SetBodyType(BodyType::kDynamic);
}

//敵の生成
GameObject* GameScene::CreateEnemy() {
	GameObject* gameObject = CreateGameObject();
	gameObject->SetName("敵");
	//Object3d
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("enemy");

	//BoxCollider
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetBodyType(BodyType::kDynamic);

	return gameObject;
}
