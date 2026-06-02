#include "PrimitiveData.h"

//初期化
void PrimitiveData::OBB::Initialize() {
	center = { 0.0f,0.0f,0.0f };
	quaternion = Quaternion::IdentityQuaternion();
	orientations[0] = { 1.0f,0.0f,0.0f };
	orientations[1] = { 0.0f,1.0f,0.0f };
	orientations[2] = { 0.0f,0.0f,1.0f };
	size = Vector3::MakeAllOne();
}
