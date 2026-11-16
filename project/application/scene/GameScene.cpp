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
#include "InputComponent.h"
#include "MovementComponent.h"
#include "JumpComponent.h"
#include "CameraControlComponent.h"
#include "ImGuiManager.h"
#include "MapChipField.h"
#include "MapChipCollider.h"
#include "Camera.h"
#include "FollowCameraController.h"
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

	//コマンドの生成
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
	//スティックの移動用コマンドを生成
	stickMoveCommand_ = std::make_unique<StickMovementCommand>();

	//ジャンプコマンドを生成
	JumpCommand_ = std::make_unique<JumpCommand>();

	//カメラコントロール用のコマンド
	stickCameraControlCommand_ = std::make_unique<StickCameraControlCommand>();

	//マップチップフィールドを生成
	mapChipField_ = MapChipField::Create(mapChipCsvName_);

	//平行光源
	GameObject* gameObject = CreateGameObject();
	gameObject->AddComponent<DirectionalLight>();
	gameObject->GetTransform().SetEulerAngleDegrees({-120.0f,0.0f,0.0f});
	gameObject->SetName("平行光源");

	//カメラ
	GameObject* cameraObject = CreateGameObject();
	cameraObject->SetName("ゲームカメラ");
	cameraObject->AddComponent<Camera>();
	followCamera_ = cameraObject->AddComponent<FollowCameraController>();
	followCamera_->SetRadius(9.0f);
	followCamera_->SetTheta(0.5f);

	//SkyBoxの生成
	CreateSkyBox();

	//フィールドの生成
	CreateField();

	//プレイヤーの作成
	playerObject_ = CreatePlayer();
	//カメラのターゲットを設定
	followCamera_->SetTarget(playerObject_);

	GameObject* wallObject = CreateGameObject();
	wallObject->GetTransform().translate.z = 1.0f;
	wallObject->AttachTo(playerObject_,false);
	Object3d* object3d = wallObject->AddComponent<Object3d>();
	object3d->SetModel("wall");
}

//更新のステート
void GameScene::UpdateState() {
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

	//各ブロックを生成
	for(uint32_t z = 0; z < mapChipData.data.size(); z++) {
		std::vector<MapChipStatus>mapChipStatusData = mapChipData.data[z];
		for(uint32_t x = 0; x < mapChipStatusData.size(); x++) {
			//ステータス
			MapChipStatus status = mapChipStatusData[x];
			for(uint32_t y = 0; y < status.height; y++) {
				//空白だった場合
				if(status.type == MapChipType::kBlank) {
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
				object3d->SetModel("debugCube");
			}
		}
	}
}

//プレイヤーの作成
GameObject* GameScene::CreatePlayer() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = CreateGameObject();
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

	//インプットコンポーネントを設定
	InputComponent* inputComponent = gameObject->AddComponent<InputComponent>();
	inputComponent->Setup(GetSceneContext().input);

	//移動用のコンポーネントを設定
	MovementComponent* movementComponent = gameObject->AddComponent<MovementComponent>();
	//ジャンプ用のコンポーネントを設定
	JumpComponent* jumpComponent = gameObject->AddComponent<JumpComponent>();
	//カメラ操作用のコンポーネントを設定
	CameraControlComponent* cameraControlComponent = gameObject->AddComponent<CameraControlComponent>();
	//リジッドボディを設定
	RigidBody* rigidBody = gameObject->AddComponent<RigidBody>();
	//移動用のコンポーネントのセットアップ
	movementComponent->Setup(rigidBody,10.0f,12.0f);
	//ジャンプ用のコンポーネントのセットアップ
	jumpComponent->Setup(rigidBody,15.0f);
	//カメラさ操作用のコンポーネントのセットアップ
	cameraControlComponent->Setup(followCamera_,3.0f);

	//スティックのコマンドを追加	
	stickMoveCommand_->SetMovementComponent(movementComponent);
	inputComponent->AddAxisInput(StickType::kLeft,std::move(stickMoveCommand_->Clone()));

	//カメラ操作用のコマンドを追加
	stickCameraControlCommand_->SetCameraControlComponent(cameraControlComponent);
	inputComponent->AddAxisInput(StickType::kRight,std::move(stickCameraControlCommand_->Clone()));

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
		//移動酔うコンポーネントを追加
		movementCommands_[i]->SetMovementComponent(movementComponent);
		//移動コマンドを設定
		inputComponent->AddInputState(static_cast<BYTE>(key),InputPhase::kHeld,std::move(movementCommands_[i]->Clone()));
	}

	//ジャンプコマンドを設定
	JumpCommand_->SetJumpComponent(jumpComponent);
	inputComponent->AddInputState(static_cast<BYTE>(DIK_SPACE),InputPhase::kPressed,std::move(JumpCommand_->Clone()));
	inputComponent->AddInputState(XboxInput::kA,InputPhase::kPressed,std::move(JumpCommand_->Clone()));

	//プレイヤーの設定
	Player* player = gameObject->AddComponent<Player>();
	player->SetObject3d(object3d);
	player->SetInput(GetSceneContext().input);

	//マップチップのコライダーを設定
	MapChipCollider* mapChipCollider = gameObject->AddComponent<MapChipCollider>();
	//マップチップコライダーのセットアップ
	mapChipCollider->Setup(mapChipField_.get(),rigidBody,{0.8f,0.8f,0.8f});

	return gameObject;
}