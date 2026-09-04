#pragma once
#include "BaseScene.h"
#include "Vector3.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :public BaseScene{
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
	/// デバッグ
	/// </summary>
	void Debug()override;
private://メンバ変数
};