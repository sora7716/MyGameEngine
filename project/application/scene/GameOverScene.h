#pragma once
#include "BaseScene.h"
#include "Vector2.h"
class Text;

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOverScene :public BaseScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameOverScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameOverScene()override;

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
	Camera* camera_ = nullptr;
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//ゲームオーバー
	std::unique_ptr<Text>gameOver_ = nullptr;
	Vector2 gameOverPos_ = {};
	float gameOverSize_ = 100.0f;

	//ゲームシーンに戻す
	std::unique_ptr<Text>pressReturn_ = nullptr;
	Vector2 pressStartPos_ = {};
	float pressStartSize_ = 64.0f;
};

