#pragma once
#include "BaseGround.h"

/// <summary>
/// 落ちる地面
/// </summary>
class FallingGround :public BaseGround {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	FallingGround();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FallingGround()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 衝突したら
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="other">衝突した対象</param>
	void OnCollision(uint32_t index, ColliderState* other)override;
private://定数
	//落ちるまでの秒数
	static inline const float kFallDelaySecond = 0.2f;
private://メンバ変数
	//落ちるフラグ
	bool isFalling_ = false;
	//落ちるブロック番号
	uint32_t fallingBlockIndex_ = 0;
	//落ちるまでの秒数
	float fallDelaySecond_ = 0.0f;
};

