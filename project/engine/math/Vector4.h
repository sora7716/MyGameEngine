#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <string>
struct RGB final {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 final {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	/// <summary>
	/// RGB値のゲッター
	/// </summary>
	/// <returns>rgb</returns>
	const RGB GetRGB()const;

	/// <summary>
	/// RGBのセッター
	/// </summary>
	/// <param name="rgb"></param>
	void SetRGB(const RGB& rgb);

	/// <summary>
	/// 白のゲッター
	/// </summary>
	/// <returns>白</returns>
	static Vector4 MakeWhiteColor();

	/// <summary>
	/// 赤のゲッター
	/// </summary>
	/// <returns>赤</returns>
	static Vector4 MakeRedColor();

	/// <summary>
	/// 緑のゲッター
	/// </summary>
	/// <returns>緑</returns>
	static Vector4 MakeGreenColor();

	/// <summary>
	/// 青のゲッター
	/// </summary>
	/// <returns>青</returns>
	static Vector4 MakeBlueColor();

	/// <summary>
	/// 黒のゲッター
	/// </summary>
	/// <returns>黒</returns>
	static Vector4 MakeBlackColor();

	/// <summary>
	/// カラーコードをVector4に変換
	/// </summary>
	/// <param name="colorCode">カラーコード</param>
	/// <returns>Vector4</returns>
	static Vector4 ColorCodeTransform(const std::string& colorCode);

	/// <summary>
	/// Vector3に変換
	/// </summary>
	/// <returns>Vector3</returns>
	const Vector3 ToVector3()const;

	/// <summary>
	/// Vector3をVector4に代入
	/// </summary>
	/// <param name="v">3次元ベクトル</param>
	/// <returns>4次元ベクトル</returns>
	Vector4 operator=(const Vector3& v);

	/// <summary>
	/// 加法
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>4次元ベクトル</returns>
	Vector4 operator+(const Vector4& v);

	/// <summary>
	/// 除算
	/// </summary>
	/// <param name="num">浮動小数</param>
	/// <returns>4次元ベクトル</returns>
	Vector4 operator/(float num);

	/// <summary>
	/// スカラー倍
	/// </summary>
	/// <param name="n">浮動小数</param>
	void operator*=(float n);

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="v">ベクトル</param>
	void operator+=(const Vector4& v);

	/// <summary>
	/// 乗法
	/// </summary>
	/// <param name="m">行列</param>
	/// <returns>4次元ベクトル</returns>
	Vector4 operator*(const Matrix4x4& m);
};