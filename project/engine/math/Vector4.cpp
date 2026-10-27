#include "Vector4.h"
#include <cassert>

//単位ベクトルを取得
Vector4 Vector4::GetOne(){
	return {1.0f,1.0f,1.0f,1.0f};
}

//ゼロベクトルを取得
Vector4 Vector4::GetZero(){
	return { 0.0f,0.0f,0.0f,0.0f };
}

//Vector3の設定
void Vector4::SetVector3(const Vector3& v) {
	*this = { v.x,v.y,v.z,w };
}

//Vector3の取得
const Vector3 Vector4::GetVector3() const {
	return { x,y,z };
}

// Vector3をVector4に代入
Vector4 Vector4::operator=(const Vector3& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	w = 1.0f;
	return *this;
}

//加法
Vector4 Vector4::operator+(const Vector4& v) {
	Vector4 result = {};
	result.x = x + v.x;
	result.y = y + v.y;
	result.z = z + v.z;
	result.w = w + v.w;
	return result;
}

//除算
Vector4 Vector4::operator/(float num) {
	Vector4 result = {};
	result.x = x / num;
	result.y = y / num;
	result.z = z / num;
	result.w = w / num;
	return result;
}

//スカラー倍
void Vector4::operator*=(float n) {
	x *= n;
	y *= n;
	z *= n;
	w *= n;
}

//乗算
void Vector4::operator+=(const Vector4& v) {
	*this = *this + v;
}

//乗法
Vector4 Vector4::operator*(const Matrix4x4& m) {
	Vector4 result{};

	result.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0];
	result.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1];
	result.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2];
	result.w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3];

	return result;
}
