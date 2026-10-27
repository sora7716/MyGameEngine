#pragma once
#include "Vector3.h"
#include "PhysicsData.h"
/// <summary>
/// 物理演算
/// </summary>
namespace physics {
	/// <summary>
	/// フックの法則(ばね力)
	/// </summary>
	/// <param name="spring">ばね</param>
	/// <param name="ball">ボール</param>
	/// <returns>加速度</returns>
	Vector3 ApplySpringForce(const Spring& spring, const Ball& ball);

	/// <summary>
	/// 振り子
	/// </summary>
	/// <param name="pendulum">振り子</param>
	/// <param name="ballPos">ボールの位置</param>
	/// <returns>位置</returns>
	Vector3 ApplyPendulumForce(Pendulum& pendulum, const Vector3& ballPos);

	//重力加速度
	const Vector3 kGravity = { 0.0f,-20.0f,0.0f };
};

