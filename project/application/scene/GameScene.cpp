#include "GameScene.h"
#include "GameObject.h"
#include "SkyBox.h"
#include "Object3d.h"
#include "DirectionalLight.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Player.h"
#include "InputComponent.h"
#include "MovementCommand.h"
#include "MovementComponent.h"
#include "JumpCommand.h"
#include "JumpComponent.h"
#include "ImGuiManager.h"
#include "MapChipField.h"
#include "MapChipCollider.h"
#include "Color.h"
#include <array>

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
	JumpCommand_ = std::make_unique<JumpCommand>();

	//マップチップフィールドを生成
	mapChipField_ = MapChipField::Create(mapChipCsvName_);

	//平行光源
	GameObject* gameObject = CreateGameObject();
	gameObject->AddComponent<DirectionalLight>();
	gameObject->GetTransform().SetEulerAngleDegrees({-120.0f,0.0f,0.0f});
	gameObject->SetName("平行光源");

	GameObject* groundObject = CreateGameObject();
	groundObject->GetTransform().scale = {30.0f,1.0f,30.0f};
	groundObject->GetTransform().translate = {0.0f,-0.5f,0.0f};
	groundObject->AddComponent<BoxCollider>();

	//SkyBoxの生成
	CreateSkyBox();

	//フィールドの生成
	CreateField();

	//プレイヤーの作成
	playerObject = CreatePlayer();
}

//更新のステート
void GameScene::UpdateState() {
	//プレイヤーの位置からマップ番号を取得
	indexSet_ = mapChipField_->GetMapChipIndexByPosition(playerObject->GetTransform().translate);
	//プレイヤーの位置からブロックのステータスを取得
	statusForPos_ = mapChipField_->GetMapChipStatus(playerObject->GetTransform().translate);
}

//デバッグ
void GameScene::Debug() {
#ifdef USE_IMGUI
	//基底クラスのデバッグ
	BaseScene::Debug();
	ImGui::Begin("マップチップに関する物");
	ImGui::Text("x:%d,y:%d,z:%d",indexSet_.x,indexSet_.y,indexSet_.z);
	ImGui::Text("pos:status = %s,count = %d",statusForPos_.type != MapChipType::kBlank ? "ブロック" : "空気",statusForPos_.count);
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
			for(uint32_t y = 0; y < status.count; y++) {
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
				object3d->SetModel("cube");
			}
		}
	}
}

//プレイヤーの作成
GameObject* GameScene::CreatePlayer() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = CreateGameObject();
	Vector3Int playerPosIndex = {0,1,0};
	gameObject->GetTransform().translate = mapChipField_->GetMapChipPositionByIndex(playerPosIndex);
	gameObject->SetName("プレイヤー");
	//オブジェクト3dを設定
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("player");

#ifdef _DEBUG
	//ワイヤーフレーム
	Object3d* wireFrame = gameObject->AddComponent<Object3d>();
	wireFrame->SetModel("cube");
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
	movementComponent->Setup(rigidBody,10.0f,12.0f);
	//ジャンプ用のコンポーネントのセットアップ
	jumpComponent->Setup(rigidBody,15.0f);

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
		inputComponent->AddInputState(static_cast<BYTE>(key),InputPhase::kHeld,std::move(movementCommands_[i]));
	}

	//ジャンプコマンドを設定
	JumpCommand_->SetJumpComponent(jumpComponent);
	inputComponent->AddInputState(static_cast<BYTE>(DIK_SPACE),InputPhase::kHeld,std::move(JumpCommand_));

	//プレイヤーの設定
	Player* player = gameObject->AddComponent<Player>();
	player->SetObject3d(object3d);
	player->SetInput(GetSceneContext().input);

	//マップチップのコライダーを設定
	MapChipCollider* mapChipCollider = gameObject->AddComponent<MapChipCollider>();
	//マップチップコライダーのセットアップ
	mapChipCollider->Setup(mapChipField_.get(),rigidBody,{0.8f,0.8f,0.8f});

	//コライダーを設定
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetBodyType(BodyType::kDynamic);

	return gameObject;
}