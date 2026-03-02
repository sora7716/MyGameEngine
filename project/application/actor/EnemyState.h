#pragma once

//前方宣言
class Enemy;

/// <summary>
/// エネミーの状態の基底クラス
/// </summary>
class IEnemyState {
public://メンバ変数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IEnemyState() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual~IEnemyState() = default;

	/// <summary>
	/// 敵のセッター
	/// </summary>
	/// <param name="enemy">敵</param>
	void SetEnemy(Enemy* enemy);

	/// <summary>
	/// 実行(純粋仮想関数)
	/// </summary>
	virtual void Exce() = 0;
protected://メンバ変数
	Enemy* enemy_ = nullptr;
};

/// <summary>
/// スポーン
/// </summary>
class EnemeyStateSpawn :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	void Exce()override;
};

/// <summary>
/// 待機
/// </summary>
class EnemeyStateIdol :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	void Exce()override;
};

/// <summary>
/// 追従
/// </summary>
class EnemyStateChase :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	void Exce()override;
};

///// <summary>
///// 攻撃
///// </summary>
//class EnemeyStateAttack :public IEnemyState {
//public://メンバ関数
//	/// <summary>
//	/// 実行
//	/// </summary>
//	void Exce()override;
//};
//
