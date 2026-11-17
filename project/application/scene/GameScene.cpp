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
	//ジャンプコマンドを生成
	stickMoveCommand_ = std::make_unique<StickMovementCommand>();
	//スティックの移動用コマンドを生成
	JumpCommand_ = std::make_unique<JumpCommand>();
	//カメラコントロール用のコマンド
	stickCameraControlCommand_ = std::make_unique<StickCameraControlCommand>();

	//マテリアルの生成
	std::string environmentMap = "engine/resources/textures/skybox_cube.dds";
	std::string blockTexture = "engine/resources/models/debugCube/MapChipDebugCube_BaseColor.png";
	AddMaterialInstance("defaultBlock",{blockTexture,environmentMap});
	AddMaterialInstance("highLightBlock",{blockTexture,environmentMap});

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

	//プレイヤーの作成
	playerObject_ = CreatePlayer();
	//カメラのターゲットを設定
	followCamera_->SetTarget(playerObject_);

	//オブジェクトの設置予定地
	placementPreview = PlacementPreview();
}

//更新のステート
void GameScene::UpdateState() {
	//足元にあるブロックを取得
	okeru = TryGetPlacementIndex(placementPreview->GetWorldPos(),objectIndex_);
}

//デバッグ
void GameScene::Debug() {
#ifdef USE_IMGUI
	//基底クラスのデバッグ
	BaseScene::Debug();
	ImGui::Begin("マップチップに関する物");
	if(ImGui::Button("オブジェクトの生成")&& okeru) {
		CreateDefenseObject(objectIndex_);
	}
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
	//マテリアルの変更
	MaterialInstance* highlightColor = FindMaterialInstance("highLightBlock");
	highlightColor->SetParameter("color",Color::GetRed());

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
				object3d->SetModel("debugCube");
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
	//カメラ操作用のコンポーネントを設定
	CameraControlComponent* cameraControlComponent = gameObject->AddComponent<CameraControlComponent>();
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
	//カメラさ操作用のコンポーネントのセットアップ
	float followSpeed = 5.0f;//追従カメラの補間速度
	cameraControlComponent->Setup(followCamera_,followSpeed);

	//スティックの移動コマンドを追加	
	stickMoveCommand_->SetMovementComponent(movementComponent);
	inputComponent->AddAxisInput(StickType::kLeft,std::move(stickMoveCommand_->Clone()));
	//スティックのカメラ操作用のコマンドを追加
	stickCameraControlCommand_->SetCameraControlComponent(cameraControlComponent);
	inputComponent->AddAxisInput(StickType::kRight,std::move(stickCameraControlCommand_->Clone()));
	//ジャンプコマンドを追加
	inputComponent->AddInputState(XboxInput::kA,InputPhase::kPressed,std::move(JumpCommand_->Clone()));
	//#ifdef _DEBUG
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
	//#endif // _DEBUG

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

//オブジェクトの設置予定地
GameObject* GameScene::PlacementPreview() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = CreateGameObject();
	Transform& transform = gameObject->GetTransform();
	transform.translate.z = 1.0f;
	gameObject->SetName("設置予定地");

#ifdef _DEBUG
	//モデルの追加
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("tile");
	object3d->SetMaterialParameter(0,"color",Color::GetMagenta());

	Object3d* wireframe = gameObject->AddComponent<Object3d>();
	wireframe->SetModel("cube");
	wireframe->SetFillMode(rasterizerMode::FillMode::kWireframe);
	wireframe->SetMaterialParameter(0,"color",Color::GetBlack());
#endif // _DEBUG

	//プレイヤーに親子付け
	gameObject->AttachTo(playerObject_,false);
	//親のオブジェクト
	const GameObject* parent = gameObject->GetParentObject();
	//親のリジッドボディを取得
	RigidBody* parentRigidBody = parent->GetComponent<RigidBody>();
	//親のリジッドボディがない場合
	if(!parentRigidBody) {
		return gameObject;
	}

	//マップチップのコライダーを設定
	MapChipCollider* mapChipCollider = gameObject->AddComponent<MapChipCollider>();
	//マップチップコライダーのセットアップ
	Vector3 hitBoxSize = Vector3::GetOne();
	mapChipCollider->Setup(mapChipField_.get(),parentRigidBody,hitBoxSize,true);

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
	//足元にブロックがなければ
	if(underBlockStatus.type == MapChipType::kBlank) {
		return false;
	}
	//現在のステータスを取得
	mapChipDesc::MapChipStatus currentBlockStatus = mapChipField_->GetMapChipStatusByIndex(currentIndex);
	if(currentBlockStatus.type == MapChipType::kBlock) {
		return false;
	}

	//検索キーに値を代入
	index = currentIndex;

	return true;
}

//守るオブジェクトの生成
void GameScene::CreateDefenseObject(const Vector3Int index) {
	GameObject* gameObject = CreateGameObject();
	gameObject->GetTransform().translate = mapChipField_->GetMapChipPositionByIndex(index);
	gameObject->SetName("守るオブジェクト");

	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("wall");

}
