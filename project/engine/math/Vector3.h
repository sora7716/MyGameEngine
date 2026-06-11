#pragma once
#include "Matrix4x4.h"
#include <cstdint>
/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	/// <summary>
	/// Vector3のメンバ変数すべてに1.0fを代入したVector3を作成
	/// </summary>
	/// <returns>Vector3</returns>
	static Vector3 MakeAllOne();

	/// <summary>
	/// 最小値
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>最小値</returns>
	Vector3 Min(const Vector3& v)const;

	/// <summary>
	/// 最大値
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>最大値</returns>
	Vector3 Max(const Vector3& v)const;

	/// <summary>
	/// 絶対値
	/// </summary>
	/// <returns>絶対値</returns>
	Vector3 Abs()const;

	/// <summary>
	/// 小数点切り捨て
	/// </summary>
	/// <returns>小数点切り捨て</returns>
	Vector3 Floor()const;

	/// <summary>
	/// 長さ(ノルム)
	/// </summary>
	/// <returns>長さ(ノルム)</returns>
	float Length();

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns>正規化</returns>
	Vector3 Normalize()const;

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>内積</returns>
	float Dot(const Vector3& v)const;

	/// <summary>
	/// クロス積
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>クロス積</returns>
	Vector3 Cross(const Vector3& v)const;

	/// <summary>
	/// 線形補間
	/// </summary>
	/// <param name="begin">最初のベクトル</param>
	/// <param name="end">最後のベクトル</param>
	/// <param name="frame">フレーム</param>
	/// <returns>現在のベクトル</returns>
	static Vector3 Lerp(const Vector3& begin, const Vector3& end, float frame);

	// 円関数を使用した線形補間
	//加法
	Vector3 operator+(const Vector3& v)const;
	//減法
	Vector3 operator-(const Vector3& v)const;
	//乗法
	Vector3 operator*(const Vector3& v)const;
	//乗法(行列)
	Vector3 operator*(const Matrix4x4& m)const;
	//除法
	Vector3 operator/(const Vector3& v)const;
	//加法(複合)
	Vector3& operator+=(const Vector3& v);
	//減法(複合)
	Vector3& operator-=(const Vector3& v);
	//乗法(複合)
	Vector3& operator*=(const Vector3& v);

	//除法(複合)
	Vector3& operator/=(const Vector3& v);
	// スカラー倍
	Vector3 operator*(float n)const;
	// スカラー倍(複合)
	Vector3& operator*=(float n);
	//減法(float)
	Vector3 operator-(float n)const;
	// 除法(float複合)
	Vector3& operator/=(float n);
	//除法(float)
	Vector3 operator/(float n)const;
	//加法(float)
	Vector3 operator+(float n);
	//加法(float)
	Vector3& operator+=(float n);
	//マイナスにする
	Vector3 operator-()const;
	// vのほうが小さい
	bool operator<(const Vector3& v);
};
//float*Vector3
const Vector3 operator*(float n, const Vector3& v);

/// <summary>
/// 3次元ベクトルの整数型
/// </summary>
struct Vector3Int {
	int32_t x;
	int32_t y;
	int32_t z;

	//Vector3Int同士の比較
	bool operator<(const Vector3Int& v)const;

	//Vector3Intが一致しているか
	bool operator!=(const Vector3Int& v)const;

	//Vector3からVector3Intへ変換
	Vector3Int& operator=(const Vector3& v);
};