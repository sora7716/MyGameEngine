#include "Vector2.h"
#include "Vector3.h"
#include <cmath>

//正規化
Vector2 Vector2::Normalize()const {
	Vector2 result = {};
	float len = std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f));
	if (len != 0.0f) {
		result.x = x / len;
		result.y = y / len;
	}
	return result;
}

//小数点切り捨て
Vector2 Vector2::Floor()const {
	Vector2 result = {};
	result.x = std::floor(result.x);
	result.y = std::floor(result.y);
	return result;
}

//Vector2のメンバ変数すべてに1.0fを代入したVector2を作成
Vector2 Vector2::MakeAllOne() {
	return Vector2(1.0f, 1.0f);
}

//加法
Vector2 Vector2::operator+(const Vector2& v)const {
	return { x + v.x,y + v.y };
}

//減法
Vector2 Vector2::operator-(const Vector2& v)const {
	return { x - v.x,y - v.y };
}

//乗法
Vector2 Vector2::operator*(const Vector2& v)const {
	return { x * v.x,y * v.y };
}

//除法
Vector2 Vector2::operator/(const Vector2& v)const {
	return { x / v.x,y / v.y };
}

//加法(複合)
Vector2& Vector2::operator+=(const Vector2& v) {
	// TODO: return ステートメントをここに挿入します
	x += v.x;
	y += v.y;
	return *this;
}

//減法(複合)
Vector2& Vector2::operator-=(const Vector2& v) {
	// TODO: return ステートメントをここに挿入します
	x -= v.x;
	y -= v.y;
	return *this;
}

//乗法(複合)
Vector2& Vector2::operator*=(const Vector2& v) {
	// TODO: return ステートメントをここに挿入します
	x *= v.x;
	y *= v.y;
	return *this;
}

//除法(複合)
Vector2& Vector2::operator/=(const Vector2& v) {
	// TODO: return ステートメントをここに挿入します
	x /= v.x;
	y /= v.y;
	return *this;
}

//スカラー倍
Vector2 Vector2::operator*(float n) const {
	return { x * n,y * n };
}

//除算
Vector2 Vector2::operator/(float n) const {
	Vector2 result = {};
	result.x = x / n;
	result.y = y / n;
	return result;
}

//スカラー倍複合
Vector2& Vector2::operator*=(float n) {
	// TODO: return ステートメントをここに挿入します
	x *= n;
	y *= n;
	return *this;
}

//Vector3を代入
Vector2& Vector2::operator=(const Vector3& v) {
	// TODO: return ステートメントをここに挿入します
	x = v.x;
	y = v.y;
	return *this;
}

//Vector2Int同士の比較
bool Vector2Int::operator<(const Vector2Int& v) const {
	if (x != v.x) {
		return x < v.x;
	}
	return y < v.y;
}

//Vector2Int同士が一致してないか
bool Vector2Int::operator!=(const Vector2Int& v) const {
	return v.x != x || v.y != y;
}

//Vector2からVector3へ変換
Vector2Int& Vector2Int::operator=(const Vector2& v) {
	// TODO: return ステートメントをここに挿入します
	x = static_cast<int32_t>(v.x);
	y = static_cast<int32_t>(v.y);
	return *this;
}
