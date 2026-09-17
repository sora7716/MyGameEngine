#pragma once
#include "IEnemyState.h"
#include "Quaternion.h"
#include "Vector3.h"

//前方宣言
class GameObject;
class RigidBody;

/// <summary>
/// ダメージを受けたとき
/// </summary>
class EnemyDamageState :public IEnemyState{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyDamageState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyDamageState()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵</param>
	void Initialize(Enemy* enemy)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ関数
	/// <summary>
	/// のけぞりアクションの初期化
	/// </summary>
	void InitializeFlinch();

	/// <summary>
	/// のけぞりアクションの更新
	/// </summary>
	void UpdateFlinch();

	/// <summary>
	/// ノックバックの初期化
	/// </summary>
	void InitializeKnockback();

	/// <summary>
	/// ノックバックの更新
	/// </summary>
	void UpdateKnockback();
private://定数
	//のけぞりの最大時間
	static inline const float kFlinchDuration = 0.12f;
	//のけぞるときの角度<度>
	static inline const float kFlinchAngle = 30.0f;

	//ノックバックの最大時間
	static inline const float kKnockbackDuration = 0.25f;
	//ノックバックの速度
	static inline const float kKnockbackSpeed = 20.0f;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//リジットボディ
	RigidBody* rigidBody_ = nullptr;

	//衝突した方向ベクトル
	Vector3 hitDirection_ = {};

	//ダメージを受けた瞬間の回転
	Quaternion normalRotate_ = {};

	//のけぞりタイマー
	float flinchTimer_ = 0.0f;
	//のけぞった時のクォータニオン
	Quaternion flinchRotate_ = {};
	bool isFinishedFlinch_ = false;

	//ノックバックタイマー
	float knockbackTimer_ = 0.0f;
	bool isFinishedKnockback_ = false;
};

