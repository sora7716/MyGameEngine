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
