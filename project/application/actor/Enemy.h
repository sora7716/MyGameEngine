#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"

//前方宣言
class RigidBody;

/// <summary>
/// 敵
/// </summary>
class Enemy :public Component{
private://構造体やenum
	//状態
	enum class Behavior :uint32_t{
		kNone,
		kNormal,
		kDamage
	};

	//ダメージを受けた時のフェーズ
	enum class DamagePhase{
		kNone,
		kDamageReaction,//ダメージを受けたときのリアクション
		kRecover//戻す
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Enemy(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 衝突したら
	/// </summary>
	/// <param name="other">衝突対象</param>
	void OnTrigger(BaseCollider* other)override;

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;
private://メンバ関数
	/// <summary>
	/// 通常状態の初期化
	/// </summary>
	void RootInitialize();

	/// <summary>
	/// 通常状態の更新
	/// </summary>
	void RootUpdate();

	/// <summary>
	/// のけぞりアクションの初期化
	/// </summary>
	void FlinchInitialize();

	/// <summary>
	/// のけぞりアクションの更新
	/// </summary>
	void FlinchUpdate();

	/// <summary>
	/// ノックバックの初期化
	/// </summary>
	void KnockbackInitialize();

	/// <summary>
	/// ノックバックの更新
	/// </summary>
	void KnockbackUpdate();

	/// <summary>
	/// ダメージリアクションの初期化
	/// </summary>
	void DamageReactionInitialize();

	/// <summary>
	/// ダメージリアクションの更新
	/// </summary>
	void DamageReactionUpdate();

	/// <summary>
	/// 浮き上がるときの初期化
	/// </summary>
	void RecoverInitialize();

	/// <summary>
	/// 浮き上がるときの更新
	/// </summary>
	void RecoverUpdate();
private://定数
	//のけぞりの最大時間
	static inline const float kFlinchDuration = 0.12f;
	//のけぞるときの角度<度>
	static inline const float kFlinchAngle = 30.0f;

	//ノックバックの最大時間
	static inline const float kKnockbackDuration = 0.25f;
	//ノックバックの速度
	static inline const float kKnockbackSpeed = 20.0f;

	//元に戻す
	static inline const float kRecoverDuration = 0.2f;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;

	//リジッドボディ
	RigidBody* rigidBody_ = nullptr;

	//敵の状態
	Behavior behavior_ = Behavior::kNone;
	//敵の状態のリクエスト
	Behavior behaviorRequest_ = Behavior::kNormal;

	//攻撃を受けた時のフェーズ
	DamagePhase damagePhase_ = DamagePhase::kNone;
	DamagePhase damagePhaseRequest_ = DamagePhase::kDamageReaction;

	//衝突した方向
	Vector3 hitDirection_ = {};

	//のけぞりタイマー
	float flinchTimer_ = 0.0f;
	//のけぞった時のクォータニオン
	Quaternion flinchRotate_ = {};
	bool isFinishedFlinch_ = false;

	//ノックバックタイマー
	float knockbackTimer_ = 0.0f;
	bool isFinishedKnockback_ = false;

	//元に戻す
	Quaternion damageReactionStartRotate_ = {};
	Quaternion recoverStartRotate_{};
	float recoverTimer_ = 0.0f;
};

