#pragma once
#include <cstdint>

struct Vector3;
/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>正規化ベクトル</returns>
	Vector2 Normalize();

	/// <summary>
	/// Vector2のメンバ変数すべてに1.0fを代入したVector2を作成
	/// </summary>
	/// <returns>Vector2</returns>
	static Vector2 MakeAllOne();

	//加法
	Vector2 operator+(const Vector2& v)const;
	//減法
	Vector2 operator-(const Vector2& v)const;
	//乗法
	Vector2 operator*(const Vector2& v)const;
	//除法
	Vector2 operator/(const Vector2& v)const;
	//加法(複合)
	Vector2& operator+=(const Vector2& v);
	//減法(複合)
	Vector2& operator-=(const Vector2& v);
	//乗法(複合)
	Vector2& operator*=(const Vector2& v);
	//除法(複合)
	Vector2& operator/=(const Vector2& v);
	//スカラー倍
	Vector2 operator*(float n)const;
	//スカラー倍(複合)
	Vector2& operator*=(float n);
	//Vector3を代入
	Vector2& operator=(const Vector3& v);
};

/// <summary>
/// 2次元ベクトルの整数型
/// </summary>
struct Vector2Int final {
	int32_t x;
	int32_t y;
};