#pragma once
#include "Framework.h"

/// <summary>
/// ゲームシステム
/// </summary>
class GameSystem:public Framework{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameSystem()=default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameSystem()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

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
};

