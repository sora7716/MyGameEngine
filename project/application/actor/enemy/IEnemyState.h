#pragma once
//前方宣言
class Enemy;

/// <summary>
/// エネミーのステートのインターフェース
/// </summary>
class IEnemyState{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IEnemyState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IEnemyState();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵</param>
	virtual void Enter(Enemy* enemy) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Exit() = 0;
protected://メンバ変数
	//敵
	Enemy* enemy_ = nullptr;
};

