#pragma once
/// <summary>
/// 4x4の行列
/// </summary>
struct Matrix4x4 final {
	float m[4][4];

	//加法
	Matrix4x4 operator+(const Matrix4x4& mat)const;
	//減法
	Matrix4x4 operator-(const Matrix4x4& mat)const;
	//乗法
	Matrix4x4 operator*(const Matrix4x4& mat)const;
	//加法(複合)
	Matrix4x4& operator+=(const Matrix4x4& mat);
	//減法(複合)
	Matrix4x4& operator-=(const Matrix4x4& mat);
	
	/// <summary>
	/// 逆行列
	/// </summary>
	/// <returns>逆行列</returns>
	Matrix4x4 Inverse()const;
	
	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns>転置行列</returns>
	Matrix4x4 Transpose();

	/// <summary>
	/// 逆転置行列
	/// </summary>
	/// <returns>逆転置行列</returns>
	Matrix4x4 InverseTranspose();
	
	/// <summary>
	/// 単位行列
	/// </summary>
	/// <returns>単位行列</returns>
	static Matrix4x4 Identity4x4();
};