#pragma once
#include "IState.h"
#include "PrimitiveData.h"

//前方宣言
class BowTower;

/// <summary>
/// 弓の攻撃ステート
/// </summary>
class BowTowerAttackState :public IState {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BowTowerAttackState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BowTowerAttackState()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void Setup(GameObject* gameObject)override;

	/// <summary>
	/// 入った瞬間
	/// </summary>
	void Enter()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Exit()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns>複製</returns>
	std::unique_ptr<IState>Clone()override;
private://メンバ関数
	/// <summary>
	/// 攻撃するときのアニメーションの初期化
	/// </summary>
	void InitializeAttackAnimation();

	/// <summary>
	///攻撃するときのアニメーションの更新
	/// </summary>
	void UpdateAttackAnimation();
private://定数
	//反動の最大時間
	static inline const float kRecoilMaxSecond = 0.05f;
	//立て直しの最大時間
	static inline const float kRecoverMaxSecond = 0.2f;
private://メンバ変数
	//弓タワー
	BowTower* bowTower_ = nullptr;
	//攻撃範囲
	float attackRangeAngle_ = 30.0f;
	//攻撃時間を管理するタイマー
	float attackTimer_ = 0.0f;
	//攻撃のインターバル
	float attackInterval_ = 1.0f;
	//反動を管理するタイマー
	float recoilTimer_ = 0.0f;
	//反動の最大角度
	float recoilMaxDegrees = 20.0f;
};

