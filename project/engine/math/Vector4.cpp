#include "Vector4.h"

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

// Vector3をVector4に代入
Vector4 Vector4::operator=(const Vector3& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}
