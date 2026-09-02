#include "RenderingData.h"

//オイラー角の設定
void Transform::SetEulerAngle(const Vector3& rotate){
	quaternion = Quaternion::MakeQuaternionForEulerAngle(rotate);
}
