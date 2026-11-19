#include "Physics.h"
#include "MathUtility.h"
#include <cmath>

//フックの法則(ばね力)
Vector3 physics::ApplySpringForce(const Spring& spring,const Ball& ball) {
	//加速度
	Vector3 result = ball.physicsData.acceleration;
	//ばねのアンカーとボールの位置の差
	Vector3 diff = ball.position - spring.anchor;
	float length = diff.Length();
	if(length != 0.0f) {
		Vector3 direction = diff.Normalize();
		Vector3 restPosition = spring.anchor + direction * spring.naturalLength;
		Vector3 displacement = (ball.position - restPosition) * length;
		Vector3 restoringForce = displacement * -spring.stiffness;
		//減衰抵抗を計算する
		Vector3 dampingForce = ball.physicsData.velocity * -spring.dampingCoefficient;
		//減衰抵抗も加味して、物体にかかる力を決定する
		Vector3 force = restoringForce + dampingForce;
		result = force / ball.mass;
	}
	return result;
}

//振り子
Vector3 physics::ApplyPendulumForce(Pendulum& pendulum,const Vector3& ballPos) {
	Vector3 result = ballPos;
	pendulum.angularAcceleration = -(std::fabs(physics::kGravity.y) / pendulum.length) * std::sin(pendulum.angle);
	pendulum.angularVelocity += pendulum.angularAcceleration * mathUtility::GetDeltaTime();
	pendulum.angle += pendulum.angularVelocity * mathUtility::GetDeltaTime();
	// 振り子の先端
	result.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
	result.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
	result.z = pendulum.anchor.z;
	return result;
}

//空気抵抗
Vector3 physics::AirResistance(const Vector3& velocity,float mass,float k) {
	Vector3 result{};
	// 速度の大きさ（ノルム）を計算
	float len = velocity.Length();

	// 速度がゼロでない場合のみ空気抵抗を計算
	if(len > 0.0f) {
		// 空気抵抗の力を計算 (速度の二乗に比例)
		Vector3 airResistance = -k * std::pow(len,2.0f) * velocity.Normalize();

		// 空気抵抗による加速度を計算
		Vector3 airResistanceAcceleration = airResistance / mass;

		// 総合加速度に空気抵抗と重力を加算
		result = kGravity + airResistanceAcceleration;
	}
	return result;
}

//摩擦
Vector3 physics::Friction(const Vector3& velocity,float mass,float miu) {
	Vector3 result{};
	// 動いていたら
	if(std::fabs(velocity.x) > 0.01f || std::fabs(velocity.y) > 0.01f || std::fabs(velocity.z) > 0.01f) {
		// 摩擦力の大きさを計算
		float magnitude = miu * (-mass * kGravity).Length();

		// 摩擦力の向き（速度の逆方向）
		Vector3 direction = velocity.Normalize();

		// 摩擦力を計算
		Vector3 frictionalForce = magnitude * direction;

		// 加速度に摩擦力を反映（力を質量で割る）
		result += frictionalForce / mass;

		// 摩擦力によって速度がゼロになる場合、速度と加速度を停止
		float deltaTime = mathUtility::GetDeltaTime();
		if(std::fabs(frictionalForce.x * deltaTime) > std::fabs(velocity.x) ||
			std::fabs(frictionalForce.y * deltaTime) > std::fabs(velocity.y) ||
			std::fabs(frictionalForce.z * deltaTime) > std::fabs(velocity.z)) {
			result = velocity * 60.0f;
		}
	}
	return result;
}