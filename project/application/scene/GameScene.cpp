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
#include "InputComponent.h"
#include "MovementComponent.h"
#include "JumpComponent.h"
#include "CameraControlComponent.h"
#include "PlacementPreviewComponent.h"
#include "DefenseRotationComponent.h"
#include "PlacementSwitchComponent.h"
#include "MapChipField.h"
#include "MapChipCollider.h"
#include "Camera.h"
#include "FollowCameraController.h"
#include "ImGuiManager.h"
#include "MathUtility.h"
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
	//設置予定地をひょじする用のコマンド
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

	//マテリアルの生成
	std::string environmentMap = "engine/resources/textures/skybox_cube.dds";
	std::string blockTexture = "engine/resources/models/block/SimpleStoneBlock_BaseColor.png";
	AddMaterialInstance("defaultBlock",{blockTexture,environmentMap});

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

	//守るオブジェクトモデル名のリストを設定
	defenseObjectModelNameList_.push_back("wall");
	defenseObjectModelNameList_.push_back("thunderTower");
	defenseObjectModelNameList_.push_back("freezeTower");
	defenseObjectModelNameList_.push_back("bowTower");
	defenseObjectModelNameList_.push_back("gameMine");

	//オブジェクトの設置予定地の生成
	CreatePlacementPreview();

	//プレイヤーの作成
	playerObject_ = CreatePlayer();
	//カメラのターゲットを設定
	followCamera_->SetTarget(playerObject_);
}

//更新のステート
void GameScene::UpdateState() {
	//モデルが切り替わったら回転をリセット
	if(placementSwitchComponent_->IsSwitched()) {
		defenseRotationComponent_->ResetRotation();
	}

	//オブジェクトの設置予定地の更新
	UpdatePlacementPreviewObject();
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

	//カメラ操作用のコンポーネントを設定
	cameraControlComponent_ = gameObject->AddComponent<CameraControlComponent>();
	//カメラさ操作用のコンポーネントのセットアップ
	float followSpeed = 5.0f;//追従カメラの補間速度
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
				object3d->SetMaterialInstance(0,GetMaterialInstance("defaultBlock"));

				fieldBlocks_[z][x][y] = gameObject;
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

	//リジッドボディを設定
	RigidBody* rigidBody = gameObject->AddComponent<RigidBody>();
	//移動用のコンポーネントのセットアップ
	float moveSpeed = 7.5f;//移動速度
	float moveResponse = 6.0f;//移動した際のレスポンス
	movementComponent->Setup(rigidBody,moveSpeed,moveResponse);
	movementComponent->SetCameraObject(cameraObject_);
	//ジャンプ用のコンポーネントのセットアップ
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
	placementPreviewCommand_->SetPlacementPreviewComponent(placementPreviewComponent_);
	//設置予定地の表示/非表示コマンドを追加
	inputComponent->AddInputState(XboxInputType::kB,InputPhase::kPressed,std::move(placementPreviewCommand_->Clone()));

	//設置オブジェクトの回転
	for(uint32_t i = 0; i < static_cast<uint32_t>(DirectionType::kCount); i++) {
		//オブジェクト回転用コンポーネントを設定
		defenseRotationCommands_[i]->SetDefenseRotationComponent(defenseRotationComponent_);
	}
	//左回転
	inputComponent->AddInputState(XboxInputType::kLB,InputPhase::kPressed,std::move(defenseRotationCommands_[static_cast<uint32_t>(DirectionType::kLeft)]));
	//右回転
	inputComponent->AddInputState(XboxInputType::kRB,InputPhase::kPressed,std::move(defenseRotationCommands_[static_cast<uint32_t>(DirectionType::kRight)]));


	//設置オブジェクトの切り替え
	for(uint32_t i = 0; i < static_cast<uint32_t>(DirectionType::kCount); i++) {
		//設置オブジェクトの切り替えを設定
		placementSwitchCommands_[i]->SetPlacementSwitchComponent(placementSwitchComponent_);
	}
	//左回転
	inputComponent->AddInputState(XboxInputType::kDPadLeft,InputPhase::kPressed,std::move(placementSwitchCommands_[static_cast<uint32_t>(DirectionType::kLeft)]));
	//右回転
	inputComponent->AddInputState(XboxInputType::kDPadRight,InputPhase::kPressed,std::move(placementSwitchCommands_[static_cast<uint32_t>(DirectionType::kRight)]));

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
		//移動用コンポーネントを追加
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

	return gameObject;
}

//オブジェクトの設置予定地の生成
GameObject* GameScene::CreatePlacementPreview() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = CreateGameObject();;
	Transform& transform = gameObject->GetTransform();
	transform.translate.z = 1.0f;
	gameObject->SetName("設置予定地");

	//Object3dの設定
	placementPreviewObject3d_ = gameObject->AddComponent<Object3d>();

	//設置予定地の表示/非表示用のコンポーネントを設定
	placementPreviewComponent_ = gameObject->AddComponent<PlacementPreviewComponent>();

	//設置物の回転用コンポーネントを設定
	defenseRotationComponent_ = gameObject->AddComponent<DefenseRotationComponent>();

	//設置物の切り替え用コンポーネントを設定
	placementSwitchComponent_ = gameObject->AddComponent<PlacementSwitchComponent>();
	//設置物の切り替え用コンポーネントのセットアップ
	placementSwitchComponent_->Setup(defenseObjectModelNameList_);

	return gameObject;
}

//オブジェクトのおける位置のインデックスを取得
bool GameScene::TryGetPlacementIndex(const Vector3& position,Vector3Int& index) {
	//今いるインデックスを取得
	Vector3Int currentIndex = mapChipField_->GetMapChipIndexByPosition(position);
	//足元のインデックスを取得
	Vector3Int underIndex = {currentIndex.x,currentIndex.y - 1,currentIndex.z};
	//足元のステータスを取得
	mapChipDesc::MapChipStatus underBlockStatus = mapChipField_->GetMapChipStatusByIndex(underIndex);
	//検索キーに値を代入
	index = currentIndex;

	//足元にブロックがなければ
	if(underBlockStatus.type == MapChipType::kBlank) {
		return false;
	}
	//現在のステータスを取得
	mapChipDesc::MapChipStatus currentBlockStatus = mapChipField_->GetMapChipStatusByIndex(currentIndex);
	if(currentBlockStatus.type == MapChipType::kBlock) {
		return false;
	}
	return true;
}

//オブジェクトの設置予定地の更新
void GameScene::UpdatePlacementPreviewObject() {
	//設置オブジェクトの回転が行えるかを設定
	defenseRotationComponent_->SetIsRunning(placementPreviewComponent_->GetEnabledRequest());
	//オブジェクトの切り替えが行えるかを設定
	placementSwitchComponent_->SetIsRunning(placementPreviewComponent_->GetEnabledRequest());

	//そもそも表示されて無ければ
	if(!placementPreviewComponent_->GetEnabledRequest()) {
		return;
	}

	//ブロックのおける位置を探す
	Vector3 searchPos = Vector3::GetForward() * playerObject_->GetWorldMatrix();
	isPlacementEnabled_ = TryGetPlacementIndex(searchPos,placementPreviewIndex_);

	//モデルを設定
	placementPreviewComponent_->SetModel(placementSwitchComponent_->GetCurrentModelName());

	//ブロックのおけるワールド座標の取得
	Vector3 placementPreviewWorldPos = mapChipField_->GetMapChipPositionByIndex(placementPreviewIndex_);

	//オブジェクトのBlendを切り替え
	placementPreviewObject3d_->SetBlendMode(BlendMode::kAdd);

	//マテリアルの色を設定
	Color materialColor = Color::GetRed();

	//おける位置煮る場合
	if(isPlacementEnabled_) {
		//おける位置にいた場合マテリアルを変更
		materialColor = Color::GetGreen();
	}

	//各メッシュにカラーを設定
	for(uint32_t i = 0; i < placementPreviewObject3d_->GetMaterialInstances().size(); i++) {
		placementPreviewObject3d_->SetMaterialParameter(i,"color",materialColor);
	}

	//座標を設定
	placementPreviewComponent_->SetPosition(placementPreviewWorldPos);
}
