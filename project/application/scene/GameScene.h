#pragma once
#include "BaseScene.h"
#include "RenderingData.h"
#include "PrimitiveData.h"

//前方宣言
class BaseGround;
class Player;
class GameCamera;
class StageTimer;

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
	/// <param name="sceneContext">シーンで必要なもの</param>
	void Initialize(const SceneContext& sceneContext)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	//ゲームシーンのカメラ
	Camera* camera_ = nullptr;
	//ゲームカメラ
	std::unique_ptr<GameCamera>gameCamera_ = nullptr;
	//地面
	std::unique_ptr<BaseGround>ground_ = nullptr;
	//落ちる床
	std::unique_ptr<BaseGround>fallingGround_ = nullptr;
	//ジャンプパッド
	std::unique_ptr<BaseGround>jumpPad_ = nullptr;
	//シーソーのような床
	std::unique_ptr<BaseGround>seesawPlatform_ = nullptr;
	//プレイヤー
	std::unique_ptr<Player>player_ = nullptr;
	//ステージタイマー
	std::unique_ptr<StageTimer>stageTimer_ = nullptr;
};

