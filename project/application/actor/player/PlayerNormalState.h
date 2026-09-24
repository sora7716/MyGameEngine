#pragma once
#include "IPlayerState.h"

/// <summary>
/// 通常状態
/// </summary>
class PlayerNormalState :public IPlayerState{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerNormalState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerNormalState()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player)override;

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
	/// 通常状態の初期化
	/// </summary>
	void InitializeNormal();

	/// <summary>
	/// 通常状態の更新
	/// </summary>
	void UpdateNormal();
private://メンバ変数
	//通常状態
	float bobTimer_ = 0.0f;
	//全体
	float bobRootAmplitude_ = 0.15f;
	float bobRootSpeed_ = 5.0f;
	//体
	float bobBodyAmplitude_ = 0.1f;
	float bobBodySpeed_ = 5.0f;
	//両腕
	float bobArmAmplitude_ = 0.3f;
	float bobArmSpeed_ = 5.0f;
};

