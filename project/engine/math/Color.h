#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <string>
struct RGB final{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

/// <summary>
/// 色
/// </summary>
struct Color final{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;

	/// <summary>
	/// RGB値の取得
	/// </summary>
	/// <returns>rgb</returns>
	const RGB GetRGB()const;

	/// <summary>
	/// RGBの設定
	/// </summary>
	/// <param name="rgb"></param>
	void SetRGB(const RGB& rgb);

	/// <summary>
	/// 白の取得
	/// </summary>
	/// <returns>白</returns>
	static Color GetWhite();

	/// <summary>
	/// 赤の取得
	/// </summary>
	/// <returns>赤</returns>
	static Color GetRed();

	/// <summary>
	/// 緑の取得
	/// </summary>
	/// <returns>緑</returns>
	static Color GetGreen();

	/// <summary>
	/// 青の取得
	/// </summary>
	/// <returns>青</returns>
	static Color GetBlue();

	/// <summary>
	/// 黒の取得
	/// </summary>
	/// <returns>黒</returns>
	static Color GetBlack();

	/// <summary>
	/// カラーコードをVector4に変換
	/// </summary>
	/// <param name="colorCode">カラーコード</param>
	/// <returns>Vector4</returns>
	static Color ColorCodeTransform(const std::string& colorCode);

	/// <summary>
	/// 加法
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>4次元ベクトル</returns>
	Color operator+(const Color& v);

	/// <summary>
	/// 除算
	/// </summary>
	/// <param name="num">浮動小数</param>
	/// <returns>4次元ベクトル</returns>
	Color operator/(float num);

	/// <summary>
	/// スカラー倍
	/// </summary>
	/// <param name="n">浮動小数</param>
	void operator*=(float n);

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="v">ベクトル</param>
	void operator+=(const Color& v);
};