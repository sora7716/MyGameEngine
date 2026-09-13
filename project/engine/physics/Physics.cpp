#include "Physics.h"
#include "MathUtility.h"
#include <cmath>

//フックの法則(ばね力)
Vector3 ApplySpringForce(const Spring& spring, const Ball& ball){
	//加速度
	Vector3 result = ball.physicsData.acceleration;
	//ばねのアンカーとボールの位置の差
	Vector3 diff = ball.position - spring.anchor;
	float length = diff.Length();
	if (length != 0.0f){
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
Vector3 ApplyPendulumForce(Pendulum& pendulum, const Vector3& ballPos){
	Vector3 result = ballPos;
	pendulum.angularAcceleration = -(std::fabs(physics::kGravity.y) / pendulum.length) * std::sin(pendulum.angle);
	pendulum.angularVelocity += pendulum.angularAcceleration * mathUtility::kDeltaTime;
	pendulum.angle += pendulum.angularVelocity * mathUtility::kDeltaTime;
	// 振り子の先端
	result.x = pendulum.anchor.x + sin(pendulum.angle) * pendulum.length;
	result.y = pendulum.anchor.y - cos(pendulum.angle) * pendulum.length;
	result.z = pendulum.anchor.z;
	return result;
}
