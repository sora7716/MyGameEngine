#pragma once
#include "IPlayerState.h"
#include <array>

/// <summary>
/// 攻撃状態
/// </summary>
class PlayerAttackState :public IPlayerState{
private://列挙型
	//攻撃フェーズ
	enum class AttackPhase :uint32_t{
		kWindup,
		kSwing,
		kCount
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerAttackState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerAttackState()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void Enter()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 解放
	/// </summary>
	void Exit()override;
private://メンバ関数
	/// <summary>
	/// 振りかぶり状態の初期化
	/// </summary>
	void InitializeWindup();

	/// <summary>
	/// 振りかぶり状態の更新
	/// </summary>
	void UpdateWindup();

	/// <summary>
	/// 振り下ろし状態の初期化
	/// </summary>
	void InitializeSwing();

	/// <summary>
	/// 振り下ろし状態の更新
	/// </summary>
	void UpdateSwing();
private://定数
	//振りかぶり状態の時間
	static inline const float kWindupDuration = 0.3f;
	//振り下げ状態の時間
	static inline const float kSwingDuration = 0.15f;
private://メンバ関数ポインタの配列
	//攻撃状態のテーブルの型
	using AttackPhaseFunc = void (PlayerAttackState::*)();
	//初期化のテーブル
	static std::array<AttackPhaseFunc, 2>initializeTable;
	//更新のテーブル
	static std::array<AttackPhaseFunc, 2>updateTable;
private://メンバ変数
	//攻撃フェーズ
	AttackPhase attackPhase_ = AttackPhase::kCount;
	//攻撃フェーズのリクエスト
	AttackPhase attackPhaseRequest_ = AttackPhase::kWindup;

	//振りかぶり状態
	float windupTimer_ = 0.0f;
	//振り下げ状態
	float swingTimer_ = 0.0f;
};

