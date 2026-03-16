#include "RenderingData.h"

//初期化
void TransformData::Initialize() {
	scale = Vector3::MakeAllOne();
	quaternion = Quaternion::IdentityQuaternion();
	translate = {};
	//デバッグ用
	axis = { 0.0f,1.0f,0.0f };
	angle = 0.0f;
	eulerAngle = {};
	isUsingQuaternion = false;
}
