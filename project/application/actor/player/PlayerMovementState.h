#pragma once
#include "IPlayerState.h"

/// <summary>
/// 移動状態
/// </summary>
class PlayerMovementState :public IPlayerState{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerMovementState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerMovementState()override;

	/// <summary>
	/// 初期化
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
private://メンバ関数
	/// <summary>
	/// 移動状態の初期化
	/// </summary>
	void InitializeMoving();

	/// <summary>
	/// 移動状態の更新
	/// </summary>
	void UpdateMoving();
private://メンバ変数
	//移動状態
	float movingTimer_ = 0.0f;
	//全体
	float movingRootAmplitude_ = 0.4f;
	float movingRootSpeed_ = 5.0f;
	//両腕
	float movingArmAmplitude_ = 0.8f;
	float movingArmSpeed_ = 5.0f;
};

