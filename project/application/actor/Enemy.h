#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"

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
		kReaction,//のけぞり
		kKnockback,//ノックバック
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
	void HitReactionInitialize();

	/// <summary>
	/// のけぞりアクションの更新
	/// </summary>
	void HitReactionUpdate();

	/// <summary>
	/// ノックバックの初期化
	/// </summary>
	void KnockbackInitialize();

	/// <summary>
	/// ノックバックの更新
	/// </summary>
	void KnockbackUpdate();

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
	static inline const float kReactionDuration = 0.5f;
	//のけぞるときの角度<度>
	static inline const float kReactionAngle = 45.0f;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;

	//敵の状態
	Behavior behavior_ = Behavior::kNormal;
	//敵の状態のリクエスト
	Behavior behaviorRequest_ = Behavior::kNone;

	//攻撃を受けた時のフェーズ
	DamagePhase damagePhase_ = DamagePhase::kReaction;

	//のけぞりタイマー
	float reactionTimer_ = 0.0f;
	//衝突した方向
	Vector3 hitDirection_ = {};
	//のけぞった時のクォータニオン
	Quaternion reactionQuaternion_ = {};
};

