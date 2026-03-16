#pragma once
#include "IScene.h"
#include "RenderingData.h"
#include "PrimitiveData.h"

//前方宣言
class Ground;
class Player;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :public IScene {
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
	//地面
	std::unique_ptr<Ground>ground_ = nullptr;
	//プレイヤー
	std::unique_ptr<Player>player_ = nullptr;
};

