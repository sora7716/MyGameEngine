#pragma once
#include "BaseScene.h"
#include <vector>
#include <cstdint>

//前方宣言
class Object3d;
class BoxCollider;
class CreateFieldSystem;
class FollowCameraController;
class CameraControlComponent;
class DefensePlacementSystem;
class MovementCommand;
class JumpCommand;

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
	/// プレイヤーの生成
	/// </summary>
	void CreatePlayer();

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <returns>敵</returns>
	GameObject* CreateEnemy();
private://メンバ変数
	//移動用のコマンド
#ifdef _DEBUG
	std::vector<std::unique_ptr<MovementCommand>> movementCommands_;
#endif // _DEBUG
	//ジャンプ用のコマンド
	std::unique_ptr<JumpCommand>jumpCommand_ = nullptr;

	//フィールド生成のシステム
	std::unique_ptr<CreateFieldSystem>createFieldSystem_ = nullptr;

	//プレイヤーのオブジェクト
	GameObject* playerObject_ = nullptr;

	//敵のオブジェクト
	GameObject* enemyObject_ = nullptr;

	//防衛オブジェクトの配置システム
	std::unique_ptr<DefensePlacementSystem> defensePlacementSystem_ = nullptr;

	//カメラのオブジェクト
	GameObject* cameraObject_ = nullptr;
	//追従カメラ
	FollowCameraController* followCamera_ = nullptr;
	CameraControlComponent* cameraControlComponent_ = nullptr;
};