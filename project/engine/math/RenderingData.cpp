#include "RenderingData.h"

//初期化
void TransformData::Initialize() {
	scale = Vector3::MakeAllOne();
	eulerAngle = {};
	quaternion = Quaternion::IdentityQuaternion();
	translate = {};
}
