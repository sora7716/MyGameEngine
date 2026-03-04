#pragma once
//クォータニオン
struct Quaternion {
	float x;
	float y;
	float z;
	float w;

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
	/// ノルム
	/// </summary>
	/// <returns>ノルム</returns>
	float Norm()const;

	/// <summary>
	/// 逆Quaternion
	/// </summary>
	/// <returns>逆Wuaternion</returns>
	Quaternion Inverse()const;

	/// <summary>
	/// 単位Quaternion
	/// </summary>
	/// <returns>単位Quaternion</returns>
	Quaternion Normalize()const;

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="quoternion">クオータニオン</param>
	/// <returns>クオータニオン</returns>
	Quaternion operator*(const Quaternion& quoternion)const;

	/// <summary>
	/// 除算(float)
	/// </summary>
	/// <param name="num">浮動小数</param>
	/// <returns>浮動小数</returns>
	Quaternion operator/(float num)const;
};


