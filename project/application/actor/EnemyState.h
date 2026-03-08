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
	/// <param name="gameObject">ゲームオブジェクト</param>
	virtual void Exce(GameObject& gameObject) = 0;

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

	/// <summary>
	/// コライダーのセッター
	/// </summary>
	/// <param name="colliderPtr">コライダーのポインタ</param>
	void SetColliderPtr(Collider* colliderPtr);

	/// <summary>
	/// 基準となるyawのセッター
	/// </summary>
	/// <param name="baseYaw">基準となるyaw</param>
	void SetBaseYaw(float baseYaw);
protected://メンバ関数
	/// <summary>
	/// 前方に動かす
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <param name="speed">移動速度</param>
	/// <returns>前方に動かす</returns>
	Vector3 MoveForward(const Quaternion& quaternion, float speed);
protected://メンバ変数
	//敵のスポーン位置のテーブル
	Vector3 enemySpawnPos_ = {};
	//ターゲットの位置
	Vector3 targetPos_ = {};
	//コライダー
	Collider* colliderPtr_ = nullptr;

	float baseYaw_ = 0.0f;
};

/// <summary>
/// スポーン
/// </summary>
class EnemeyStateSpawn :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void Exce(GameObject& gameObject)override;
};

/// <summary>
/// 待機
/// </summary>
class EnemeyStateIdol :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void Exce(GameObject& gameObject)override;
private://メンバ変数
	//回転時間
	float rotateTime_ = 0.0f;
};

/// <summary>
/// 追従
/// </summary>
class EnemyStateChase :public IEnemyState {
public://メンバ関数
	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void Exce(GameObject& gameObject)override;
private://メンバ関数
	/// <summary>
	/// ターゲットの方向を向く
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	Quaternion EnemyToTarget(const GameObject& gameObject);
private://メンバ変数
	//移動速度
	float moveSpeed_ = 0.05f;
};

/// <summary>
/// パトロール
/// </summary>
class EnemyStatePatrol :public IEnemyState {
public://メンバ関数
	/// <summary>
    /// 実行
    /// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void Exce(GameObject& gameObject)override;
private://メンバ変数
	//移動速度
	float moveSpeed_ = 0.05f;
};

/// <summary>
/// 突進
/// </summary>
class EnemyStateCharge :public IEnemyState {
public://メンバ関数
	/// <summary>
    /// 実行
    /// </summary>
    /// <param name="gameObject">ゲームオブジェクト</param>
	void Exce(GameObject& gameObject)override;
private://メンバ変数
	//移動速度
	float moveSpeed_ = 0.0f;
	//チャージする時間
	float chargeTime_ = 0.0f;
	//チャージ完了時間
	float maxChargeTime_ = 0.5f;
	//チャージの開放時間
	float releaseTime_ = 0.0f;
};
//周りの敵を呼ぶ
