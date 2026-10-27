#pragma once
#include <cstdint>

struct Vector3;
/// <summary>
/// 2次元ベクトル
/// </summary>
struct Vector2 final {
	float x = 0.0f;
	float y = 0.0f;

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>正規化ベクトル</returns>
	Vector2 Normalize()const;

	/// <summary>
	/// 小数点切り捨て
	/// </summary>
	/// <returns>小数点切り捨て</returns>
	Vector2 Floor()const;

	/// <summary>
	/// 単位ベクトルを取得
	/// </summary>
	/// <returns>単位ベクトル</returns>
	static Vector2 GetOne();

	/// <summary>
	/// ゼロベクトルを取得
	/// </summary>
	/// <returns>ゼロベクトル</returns>
	static Vector2 GetZero();

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
	//除算
	Vector2 operator/(float n)const;
	//スカラー倍(複合)
	Vector2& operator*=(float n);
	//Vector3を代入
	Vector2& operator=(const Vector3& v);
};

/// <summary>
/// 2次元ベクトルの整数型
/// </summary>
struct Vector2Int final {
	int32_t x = 0;
	int32_t y = 0;

	//Vector2Int同士の比較
	bool operator<(const Vector2Int& v)const;

	//Vector2Int同士が一致してないか
	bool operator!=(const Vector2Int& v)const;

	//Vector2からVector2Intへ変換
	Vector2Int& operator=(const Vector2& v);
};