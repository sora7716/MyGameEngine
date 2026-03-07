#pragma once
#include "ActorData.h"

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
	/// 実行(純粋仮想関数)
	/// </summary>
	virtual void Exce(GameObject& gameObject, Collider& collider) = 0;

	/// <summary>
	/// 敵のスポーン位置のセッター
	/// </summary>
	/// <param name="enemySpawnPos">敵のスポーン位置</param>
	void SetEnemySpawnPos(const Vector3& enemySpawnPos);

	/// <summary>
	/// ターゲットの位置のセッター
	/// </summary>
	/// <param name="targetPos">ターゲット</param>
	void SetTargetPos(const Vector3& targetPos);
protected://メンバ変数
	//敵のスポーン位置のテーブル
	Vector3 enemySpawnPos_ = {};
	//ターゲットの位置
	Vector3 targetPos_ = {};
};

/// <summary>
/// スポーン
/// </summary>
class EnemeyStateSpawn :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	void Exce(GameObject& gameObject, Collider& collider)override;
};

/// <summary>
/// 待機
/// </summary>
class EnemeyStateIdol :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	void Exce(GameObject& gameObject, Collider& collider)override;
};

/// <summary>
/// 追従
/// </summary>
class EnemyStateChase :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	void Exce(GameObject& gameObject, Collider& collider)override;
private://メンバ関数
	/// <summary>
	/// ターゲットの方向を向く
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	float EnemyToTarget(const GameObject& gameObject);
private://メンバ変数
	//移動速度
	float moveSpeed_ = 0.05f;
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
