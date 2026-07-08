#pragma once
#include "Vector3.h"

//クォータニオン
struct Quaternion {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	/// <summary>
	/// 乗法単位元
	/// </summary>
	/// <returns>乗法単位元</returns>
	static Quaternion IdentityQuaternion();

	/// <summary>
	/// 共役
	/// </summary>
	/// <returns></returns>
	Quaternion Conjugate()const;

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="q">クォータニオン</param>
	/// <returns>内積</returns>
	float Dot(const Quaternion& q)const;

	/// <summary>
	/// ノルム
	/// </summary>
	/// <returns>ノルム</returns>
	float Norm()const;

	/// <summary>
	/// 逆クォータニオン
	/// </summary>
	/// <returns>逆クォータニオン</returns>
	Quaternion Inverse()const;

	/// <summary>
	/// 単位クォータニオン
	/// </summary>
	/// <returns>単位クォータニオン</returns>
	Quaternion Normalize()const;

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="q1">クォータニオン1</param>
	/// <param name="q2">クォータニオン2</param>
	/// <param name="t">媒介変数</param>
	/// <returns></returns>
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);

	/// <summary>
	/// オイラー角からクォータニオンを生成
	/// </summary>
	/// <param name="rotate">オイラー角</param>
	/// <returns>クォータニオン</returns>
	static Quaternion MakeQuaternionForEulerAngle(const Vector3& rotate);

	/// <summary>
	/// 加算
	/// </summary>
	/// <param name="q">クォータニオン</param>
	/// <returns>クォータニオン</returns>
	Quaternion operator+(const Quaternion& q)const;

	/// <summary>
	/// 負の数にする
	/// </summary>
	/// <returns>負の数のクォータニオン</returns>
	Quaternion operator-()const;

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="q">クォータニオン</param>
	/// <returns>クォータニオン</returns>
	Quaternion operator*(const Quaternion& q)const;

	/// <summary>
	/// 除算(float)
	/// </summary>
	/// <param name="num">浮動小数</param>
	/// <returns>浮動小数</returns>
	Quaternion operator/(float num)const;
};

/// <summary>
/// 浮動小数 * クォータニオン
/// </summary>
/// <param name="num">浮動小数</param>
/// <param name="q">クォータニオン</param>
/// <returns>クォータニオン</returns>
Quaternion operator*(float num, const Quaternion& q);


