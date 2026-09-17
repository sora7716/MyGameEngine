#include "RenderingData.h"
#include "MathUtility.h"

//オイラー角の設定
void Transform::SetEulerAngle(const Vector3& rotate){
	quaternion = Quaternion::MakeQuaternionForEulerAngle(rotate);
}

//クォータニオンの設定
void Transform::SetRotate(const Quaternion& rotate){
	quaternion = rotate.Normalize();
}

//オイラー角の取得
Vector3 Transform::GetEulerAngle(){
	return mathUtility::MakeEulerAngleForQuaternion(quaternion);
}

//補間
Transform Transform::Lerp(const Transform& transform1, const Transform& transform2, float t){
	Transform result = {};
	result.scale = Vector3::Lerp(transform1.scale, transform2.scale, t);
	result.quaternion = Quaternion::Slerp(transform1.quaternion, transform2.quaternion, t);
	result.translate = Vector3::Lerp(transform1.translate, transform2.translate, t);
	return result;
}
