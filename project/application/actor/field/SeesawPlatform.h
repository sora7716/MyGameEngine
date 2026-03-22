#pragma once
#include "BaseGround.h"
#include <ColliderData.h>
#include <cstdint>

/// <summary>
/// シーソーのような床
/// </summary>
class SeesawPlatform :public BaseGround {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SeesawPlatform();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SeesawPlatform()override;

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
	/// 衝突したら
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="other">衝突した対象</param>
	void OnCollision(uint32_t index, ColliderState* other)override;
private://メンバ変数
	//プレイヤーの位
	Vector3 playerPos_ = {};
	//衝突した地面の番号
	uint32_t collisionBlockIndex_ = 0;
	float seesawAngle_ = 0.0f;
};

