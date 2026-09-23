#include "RenderingData.h"
#include "MathUtility.h"
#include "Quaternion.h"

//オイラー角の設定
void Transform::SetEulerAngle(const Vector3& eulerAngle){
	rotate = Quaternion::EulerAngleToQuaternion(eulerAngle);
}

//オイラー角(度数法)の設定
void Transform::SetEulerAngleDegrees(const Vector3& degreeAngle){
	Vector3 radianAngle = degreeAngle * mathUtility::kRad;
	rotate = Quaternion::EulerAngleToQuaternion(radianAngle);
}

//クォータニオンの設定
void Transform::SetRotate(const Quaternion& quaternion){
	rotate = quaternion.Normalize();
}

//オイラー角の取得
Vector3 Transform::GetEulerAngle(){
	return mathUtility::MakeEulerAngleForQuaternion(rotate);
}

//補間
Transform Transform::Lerp(const Transform& transform1, const Transform& transform2, float t){
	Transform result = {};
	result.scale = Vector3::Lerp(transform1.scale, transform2.scale, t);
	result.rotate = Quaternion::Slerp(transform1.rotate, transform2.rotate, t);
	result.translate = Vector3::Lerp(transform1.translate, transform2.translate, t);
	return result;
}
