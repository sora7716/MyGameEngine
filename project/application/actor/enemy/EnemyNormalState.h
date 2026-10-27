#pragma once
#include "IEnemyState.h"

/// <summary>
/// 敵の通常状態
/// </summary>
class EnemyNormalState :public IEnemyState{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyNormalState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyNormalState()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵</param>
	void Enter(Enemy* enemy)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Exit()override;
private://メンバ変数
};

