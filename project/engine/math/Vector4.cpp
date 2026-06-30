#include "Vector4.h"
#include <cassert>

//RGBのゲッター
const RGB Vector4::GetRGB()const {
	return { x,y,z };
}

//RGBのセッター
void Vector4::SetRGB(const RGB& rgb) {
	x = rgb.r;
	y = rgb.g;
	z = rgb.b;
}

//白のゲッター
Vector4 Vector4::MakeWhiteColor() {
	return Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

//赤のゲッター
Vector4 Vector4::MakeRedColor() {
	return Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

//緑のゲッター
Vector4 Vector4::MakeGreenColor() {
	return Vector4(0.0f, 1.0f, 0.0f, 1.0f);
}

//青のゲッター
Vector4 Vector4::MakeBlueColor() {
	return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
}

//黒のゲッター
Vector4 Vector4::MakeBlackColor() {
	return Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}

//カラーコードをVector4に変換
Vector4 Vector4::ColorCodeTransform(const std::string& colorCode) {
	int32_t r = std::stoi(colorCode.substr(1, 2), nullptr, 16);
	int32_t g = std::stoi(colorCode.substr(3, 2), nullptr, 16);
	int32_t b = std::stoi(colorCode.substr(5, 2), nullptr, 16);
	int32_t alpha = (colorCode.size() == 9) ? std::stoi(colorCode.substr(7, 2), nullptr, 16) : 255;
	return Vector4(r / 255.0f, g / 255.0f, b / 255.0f, alpha / 255.0f);
}

//Vector3に変換
const Vector3 Vector4::ToVector3() const {
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

//乗法
Vector4 Vector4::operator*(const Matrix4x4& m) {
	Vector4 result{};

	result.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0];
	result.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1];
	result.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2];
	result.w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3];

	return result;
}
