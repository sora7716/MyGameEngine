#pragma once
#include "BaseScene.h"
#include "MapChipData.h"
#include "Vector3.h"
#include <string>
#include <vector>
#include <cstdint>
#include "InputType.h"

//前方宣言
class MapChipField;
class FollowCameraController;
class MovementCommand;
class JumpCommand;
class StickMovementCommand;
class StickCameraControlCommand;
class PlacementPreviewCommand;
class PlacementPreviewComponent;
class Object3d;
class DefenseRotationCommand;
class DefenseRotationComponent;

//移動用のコマンドモード
enum class MovementCommandMode : uint32_t {
	kMoveFowerd,
	kMoveBack,
	kMoveLeft,
	kMoveRight,
	kCount
};

//オブジェクトの回転用コマンドモード
enum class DefenseRotationCommandMode :uint32_t {
	kLeft,
	kRight,
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
	/// <returns>オブジェクト</returns>
	GameObject* CreatePlayer();

	/// <summary>
	/// オブジェクトの設置予定地の生成
	/// </summary>
	/// <returns>オブジェクト</returns>
	GameObject* CreatePlacementPreview();

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

	/// <summary>
	/// オブジェクトの設置予定地の更新
	/// </summary>
	void UpdatePlacementPreviewObject();
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
	//オブジェクトの設置予定地の表示のコマンド
	std::unique_ptr<PlacementPreviewCommand>placementPreviewCommand_ = nullptr;
	//オブジェクトの回転用コマンド
	std::vector<std::unique_ptr<DefenseRotationCommand>>defenseRotationCommands_;

	//守るオブジェクトのモデル名の一覧
	std::vector<std::string>defenseObjectModelNameList_;
	//守るオブジェクトのモデルの数
	uint32_t defenseObjectModelCount_ = 2;

	//プレイヤーのオブジェクト
	GameObject* playerObject_ = nullptr;
	//設置予定地のオブジェクト
	PlacementPreviewComponent* placementPreviewComponent_ = nullptr;
	GameObject* placementPreviewObject_ = nullptr;
	Object3d* placementPreviewObject3d_ = nullptr;
	Vector3Int placementPreviewIndex_ = {};
	bool isPlacementEnabled_ = false;

	//設置オブジェクトの回転
	DefenseRotationComponent* defenseRotationComponent_ = nullptr;

	//カメラのオブジェクト
	GameObject* cameraObject_ = nullptr;
	//追従カメラ
	FollowCameraController* followCamera_ = nullptr;
};