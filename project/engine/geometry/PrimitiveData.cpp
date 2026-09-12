#include "PrimitiveData.h"
#include "Vector4.h"

//初期化
void primitiveData::OBB::Initialize() {
	center = { 0.0f,0.0f,0.0f };
	quaternion = Quaternion::IdentityQuaternion();
	orientations[0] = { 1.0f,0.0f,0.0f };
	orientations[1] = { 0.0f,1.0f,0.0f };
	orientations[2] = { 0.0f,0.0f,1.0f };
	size = Vector3::GetAllOne();
}

//行列との掛け算
primitiveData::AABB primitiveData::AABB::operator*(const Matrix4x4& m)const {
	Vector4 points[8] = {
		{min.x,min.y,min.z,1.0f},
		{max.x,min.y,min.z,1.0f},
		{min.x,max.y,min.z,1.0f},
		{max.x,max.y,min.z,1.0f},

		{min.x,min.y,max.z,1.0f},
		{max.x,min.y,max.z,1.0f},
		{min.x,max.y,max.z,1.0f},
		{max.x,max.y,max.z,1.0f},
	};

	Vector4 first = points[0] * m;

	AABB result{};
	result.min = { first.x,first.y,first.z };
	result.max = result.min;

	for (uint32_t i = 1; i < 8; i++) {
		Vector4 p = points[i] * m;
		Vector3 pos = { p.x,p.y,p.z };

		result.min = result.min.Min(pos);
		result.max = result.max.Max(pos);
	}

	return result;
}
