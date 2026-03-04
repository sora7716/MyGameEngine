#include "Quaternion.h"
#include "Vector3.h"
#include <cmath>

//乗法単位元
Quaternion Quaternion::IdentityQuaternion() {
	return { 0.0f,0.0f,0.0f,1.0f };
}

//共役
Quaternion Quaternion::Conjugate() {
	Quaternion result = *this;
	result.x *= -1.0f;
	result.y *= -1.0f;
	result.z *= -1.0f;
	return result;
}

//ノルム
float Quaternion::Norm() {
	return std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f) + std::pow(w, 2.0f));
}

//逆Quaternion
Quaternion Quaternion::Inverse() {
	//共役
	Quaternion conjugate = this->Conjugate();
	//ノルム
	float norm = this->Norm();
	//逆Quaternion
	Quaternion inverse = conjugate / std::pow(norm, 2.0f);
	return inverse;
}

//単位Quaternion
Quaternion Quaternion::Normalize() {
	Quaternion normalize = *this;
	float norm = this->Norm();
	normalize = normalize / norm;
	return normalize;
}

//乗法
Quaternion Quaternion::operator*(const Quaternion& quoternion) {
	//クオータニオン
	Quaternion q = *this;
	Quaternion r = quoternion;

	//虚部(ベクトル部)
	Vector3 qv = Vector3(q.x, q.y, q.z);
	Vector3 rv = Vector3(r.x, r.y, r.z);

	//返す値
	Vector3 resultV = {};
	float resultW = 0.0f;
	resultV = qv.Cross(rv) + qv * r.w + rv * q.w;
	resultW = q.w * r.w - qv.Dot(rv);

	return { resultV.x,resultV.y,resultV.z,resultW };
}

//除算(float)
Quaternion Quaternion::operator/(float num) {
	Quaternion result;
	result.x = x / num;
	result.y = y / num;
	result.z = z / num;
	result.w = w / num;
	return result;
}
