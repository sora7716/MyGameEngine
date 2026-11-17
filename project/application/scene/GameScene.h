#pragma once
#include "BaseScene.h"
#include "MapChipData.h"
#include "Vector3.h"
#include <string>
#include <vector>

//前方宣言
class MapChipField;
class FollowCameraController;
class MovementCommand;
class JumpCommand;
class StickMovementCommand;
class StickCameraControlCommand;

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
	/// プレイヤー作成
	/// </summary>
	GameObject* CreatePlayer();

	/// <summary>
	/// オブジェクトの設置予定地
	/// </summary>
	/// <returns>設置予定地</returns>
	GameObject* PlacementPreview();

	/// <summary>
	/// オブジェクトのおける位置のインデックスを取得
	/// </summary>
	/// <param name="position">位置</param>
	/// <returns>成功したか</returns>
	bool TryGetPlacementIndex(const Vector3& position,Vector3Int& index);

	/// <summary>
	/// 守るオブジェクトの生成
	/// </summary>
	/// <param name="index">検索キー</param>
	void CreateDefenseObject(const Vector3Int index);
private://メンバ変数
	//マップチップフィールド
	std::unique_ptr<MapChipField> mapChipField_ = nullptr;
	//フィールドに配置しているブロック
	std::vector<std::vector<std::vector<GameObject*>>>fieldBlocks_;

	//マップチップのCSVの名前
	std::string mapChipCsvName_ = "map_04.csv";
	//移動用のコマンド
	std::vector<std::unique_ptr<MovementCommand>> movementCommands_;
	std::unique_ptr<StickMovementCommand>stickMoveCommand_ = nullptr;
	//ジャンプ用のコマンド
	std::unique_ptr<JumpCommand>JumpCommand_ = nullptr;
	//カメラ操作用のコマンド
	std::unique_ptr<StickCameraControlCommand>stickCameraControlCommand_ = nullptr;

	//プレイヤーのオブジェクト
	GameObject* playerObject_ = nullptr;
	//設置予定地のオブジェクト
	GameObject* placementPreview = nullptr;
	Vector3Int objectIndex_ = {};
	bool okeru = false;

	//カメラのオブジェクト
	GameObject* cameraObject_ = nullptr;
	//追従カメラ
	FollowCameraController* followCamera_ = nullptr;
};