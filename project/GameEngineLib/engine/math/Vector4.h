#pragma once
#include "Vector3.h"
#include <string>
struct RGB final{
	float r;
	float g;
	float b;
};

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 final {
	float x;
	float y;
	float z;
	float w;

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
	/// Vector3をVector4に代入
	/// </summary>
	/// <param name="v">3次元ベクトル</param>
	/// <returns>4次元ベクトル</returns>
	Vector4 operator=(const Vector3& v);
};