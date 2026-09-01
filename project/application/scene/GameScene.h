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
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://定数
	//マップ全体の幅
	static inline const Vector3Int kMapSize = { 6,3,6 };
	//ブロックの幅
	static inline const Vector3 kTileSize = { 1.0f,1.0f,1.0f };
private://メンバ変数
};