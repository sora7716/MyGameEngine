#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <string>

/// <summary>
/// 4次元ベクトル
/// </summary>
struct Vector4 final {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	/// <summary>
	/// 単位ベクトルを取得
	/// </summary>
	/// <returns>単位ベクトル</returns>
	Vector4 GetOne();

	/// <summary>
	/// ゼロベクトルを取得
	/// </summary>
	/// <returns>ゼロベクトル</returns>
	Vector4 GetZero();

	/// <summary>
	/// Vector3に設定
	/// </summary>
	/// <param name="v">ベクトル</param>
	void SetVector3(const Vector3& v);

	/// <summary>
	/// Vector3を取得
	/// </summary>
	/// <returns>Vector3</returns>
	const Vector3 GetVector3()const;

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