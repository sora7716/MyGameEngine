#pragma once
#include "engine/math/PhysicsData.h"
/// <summary>
/// 物理演算
/// </summary>
class Physics {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Physics();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Physics();

	/// <summary>
    /// フックの法則(ばね力)
    /// </summary>
    /// <param name="spring">ばね</param>
    /// <param name="ball">ボール</param>
	/// <returns>加速度</returns>
	static Vector3 ApplySpringForce(const Spring& spring,const Ball& ball);

	/// <summary>
	/// 振り子
	/// </summary>
	/// <param name="pendulum">振り子</param>
	/// <param name="ballPos">ボールの位置</param>
	/// <returns>位置</returns>
	static Vector3 ApplyPendulumForce(Pendulum& pendulum, const Vector3& ballPos);
public://定数
	//重力加速度
	static inline const float kGravity = -20.0f;
};

