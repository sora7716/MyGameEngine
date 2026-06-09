#define NOMINMAX
#include "Vector3.h"
#include <cmath>
#include <algorithm>
#include <cassert>

//Vector3のメンバ変数すべてに1.0fを代入したVector3を作成
Vector3 Vector3::MakeAllOne() {
	return Vector3(1.0f, 1.0f, 1.0f);
}

//最小値
Vector3 Vector3::Min(const Vector3& v) const {
	Vector3 result = {};
	result.x = std::min(x, v.x);
	result.y = std::min(y, v.y);
	result.z = std::min(z, v.z);

	return result;
}

//最大値
Vector3 Vector3::Max(const Vector3& v) const {
	Vector3 result = {};
	result.x = std::max(x, v.x);
	result.y = std::max(y, v.y);
	result.z = std::max(z, v.z);

	return result;
}

//絶対値
Vector3 Vector3::Abs() const {
	Vector3 result = {};
	result.x = std::abs(x);
	result.y = std::abs(y);
	result.z = std::abs(z);
	return result;
}

//長さ(ノルム)
float Vector3::Length() {
	float result = std::sqrt(Vector3(x, y, z).Dot(Vector3(x, y, z)));
	return result;
}

//正規化
Vector3 Vector3::Normalize()const {
	Vector3 result = {};
	float len = Vector3(x, y, z).Length();
	if (len != 0.0f) {
		result.x = x / len;
		result.y = y / len;
		result.z = z / len;
	}
	return result;
}

//内積
float Vector3::Dot(const Vector3& v)const {
	Vector3 tempVector = Vector3(x, y, z) * v;
	float dot = tempVector.x + tempVector.y + tempVector.z;
	return dot;
}

//クロス積
Vector3 Vector3::Cross(const Vector3& v)const {
	// TODO: return ステートメントをここに挿入します
	Vector3 result{};
	result.x = y * v.z - z * v.y;
	result.y = z * v.x - x * v.z;
	result.z = x * v.y - y * v.x;
	return result;
}

//線形補間
Vector3 Vector3::Lerp(const Vector3& begin, const Vector3& end, float frame) {
	// TODO: return ステートメントをここに挿入します
	Vector3 result = {};
	result.x = std::lerp(begin.x, end.x, frame);
	result.y = std::lerp(begin.y, end.y, frame);
	result.z = std::lerp(begin.z, end.z, frame);
	return result;
}

//加法
Vector3 Vector3::operator+(const Vector3& v)const {
	return { x + v.x,y + v.y,z + v.z };
}

//減法
Vector3 Vector3::operator-(const Vector3& v)const {
	return { x - v.x,y - v.y,z - v.z };
}

//乗法
Vector3 Vector3::operator*(const Vector3& v) const {
	return { x * v.x,y * v.y,z * v.z };
}

//乗法(行列)
Vector3 Vector3::operator*(const Matrix4x4& m) const {
	Vector3 result{};
	result.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + 1.0f * m.m[3][0];
	result.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + 1.0f * m.m[3][1];
	result.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + 1.0f * m.m[3][2];
	float w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + 1.0f * m.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

//除法
Vector3 Vector3::operator/(const Vector3& v)const {
	return { x / v.x,y / v.y,z / v.z };
}

//加法(複合)
Vector3& Vector3::operator+=(const Vector3& v) {
	// TODO: return ステートメントをここに挿入します
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//減法(複合)
Vector3& Vector3::operator-=(const Vector3& v) {
	// TODO: return ステートメントをここに挿入します
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//乗法(複合)
Vector3& Vector3::operator*=(const Vector3& v) {
	// TODO: return ステートメントをここに挿入します
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

//除法
Vector3& Vector3::operator/=(const Vector3& v) {
	// TODO: return ステートメントをここに挿入します
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

// スカラー倍
Vector3 Vector3::operator*(float n)const {
	return { x * n,y * n,z * n };
}

// スカラー倍(複合)
Vector3& Vector3::operator*=(float n) {
	// TODO: return ステートメントをここに挿入します
	x *= n;
	y *= n;
	z *= n;
	return *this;
}

Vector3 Vector3::operator-(float n) const {
	return { x - n,y - n,z - n };
}

// 除法(float複合)
Vector3& Vector3::operator/=(float n) {
	// TODO: return ステートメントをここに挿入します
	x /= n;
	y /= n;
	z /= n;
	return *this;
}

//除法(float)
Vector3 Vector3::operator/(float n)const {
	return { x / n,y / n,z / n };
}

//加法(float)
Vector3 Vector3::operator+(float n) {
	Vector3 result = {
		x + n,
		y + n,
		z + n
	};
	return result;
}

//加法(float)
Vector3& Vector3::operator+=(float n) {
	// TODO: return ステートメントをここに挿入します
	x -= n;
	y -= n;
	z -= n;
	return*this;
}

//マイナスにする
Vector3 Vector3::operator-()const {
	Vector3 result{
	-x,
	-y,
	-z,
	};
	return result;
}

// vのほうが小さい
bool Vector3::operator<(const Vector3& v) {
	return x < v.x && y < v.y && z < v.z;
}

//float*Vector3
const Vector3 operator*(float n, const Vector3& v) {
	return v * n;
}

//Vector3Int同士の比較
bool Vector3Int::operator<(const Vector3Int& v) const {
	if (x != v.x) {
		return x < v.x;
	} else if (y != v.y) {
		return y < v.y;
	} else {
		return z < v.z;
	}
}
