#pragma once
#include "BaseScene.h"
#include "MapChipData.h"
#include "Vector3.h"
#include <string>
#include <vector>
#include <cstdint>
#include "InputType.h"

//前方宣言
class Object3d;
class MapChipField;
class BoxCollider;
class FollowCameraController;
class CameraControlComponent;
class DefensePlacementSystem;
class MovementCommand;
class JumpCommand;
class StickMovementCommand;
class StickCameraControlCommand;
class PlacementPreviewCommand;
class DefenseRotationCommand;
class PlacementSwitchCommand;
class DefensePlaceCommand;

//移動用のコマンドモード
enum class MovementCommandMode : uint32_t {
	kMoveFowerd,
	kMoveBack,
	kMoveLeft,
	kMoveRight,
	kCount
};

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :public BaseScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新のステート
	/// </summary>
	void UpdateState()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug()override;

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize()override;
private://メンバ関数
	/// <summary>
	/// 平行光源の生成
	/// </summary>
	void CreateDirectionalLight();

	/// <summary>
	/// カメラの生成
	/// </summary>
	/// <returns>オブジェクト</returns>
	GameObject* CreateCamera();

	/// <summary>
	/// SkyBoxの生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateSkyBox();

	/// <summary>
	/// フィールドの生成
	/// </summary>
	void CreateField();

	/// <summary>
	/// プレイヤーの生成
	/// </summary>
	void CreatePlayer();

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <returns>敵</returns>
	GameObject* CreateEnemy();
private://メンバ変数
	//マップチップフィールド
	std::unique_ptr<MapChipField> mapChipField_ = nullptr;
	//フィールドに配置しているブロック
	std::vector<std::vector<std::vector<GameObject*>>>fieldBlocks_;

	//マップチップのCSVの名前
	std::string mapChipCsvName_ = "map_04.csv";
	//移動用のコマンド
#ifdef _DEBUG
	std::vector<std::unique_ptr<MovementCommand>> movementCommands_;
#endif // _DEBUG
	std::unique_ptr<StickMovementCommand>stickMoveCommand_ = nullptr;

	//ジャンプ用のコマンド
	std::unique_ptr<JumpCommand>jumpCommand_ = nullptr;

	//カメラ操作用のコマンド
	std::unique_ptr<StickCameraControlCommand>stickCameraControlCommand_ = nullptr;

	//オブジェクトの設置予定地の表示/非表示のコマンド
	std::unique_ptr<PlacementPreviewCommand>placementPreviewCommand_ = nullptr;

	//オブジェクトの回転用コマンド
	std::vector<std::unique_ptr<DefenseRotationCommand>>defenseRotationCommands_;

	//設置するオブジェクトの切り替え用コマンド
	std::vector<std::unique_ptr<PlacementSwitchCommand>>placementSwitchCommands_;

	//オブジェクトを設置する用のコマンド
	std::unique_ptr<DefensePlaceCommand> defensePlaceCommand_ = nullptr;

	//プレイヤーのオブジェクト
	GameObject* playerObject_ = nullptr;

	//防衛オブジェクトの配置システム
	std::unique_ptr<DefensePlacementSystem> defensePlacementSystem_ = nullptr;

	//カメラのオブジェクト
	GameObject* cameraObject_ = nullptr;
	//追従カメラ
	FollowCameraController* followCamera_ = nullptr;
	CameraControlComponent* cameraControlComponent_ = nullptr;
};