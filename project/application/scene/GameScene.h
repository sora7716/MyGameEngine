#pragma once
#include "BaseScene.h"
#include <string>
#include <vector>

//前方宣言
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
	void CreatePlayer();
private://メンバ変数
	//マップチップのCSVの名前
	std::string mapChipCsvName_ = "map_01.csv";
	//移動用のコマンド
	std::vector<std::unique_ptr<MovementCommand>> movementCommands_;
	//ジャンプ用のコマンド
	std::unique_ptr<JumpCommand>JumpCommand_ = nullptr;
};