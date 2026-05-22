#include "Matrix4x4.h"

//加法
Matrix4x4 Matrix4x4::operator+(const Matrix4x4& mat) const {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m[i][j] + mat.m[i][j];
		}
	}
	return result;
}

//減法
Matrix4x4 Matrix4x4::operator-(const Matrix4x4& mat) const {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m[i][j] - mat.m[i][j];
		}
	}
	return result;
}

//乗法
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m[i][k] * mat.m[k][j];
			}
		}
	}
	return result;
}

//加法(複合)
Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& mat) {
	// TODO: return ステートメントをここに挿入します
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] += mat.m[i][j];
		}
	}
	return *this;
}

//減法(複合)
Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& mat) {
	// TODO: return ステートメントをここに挿入します
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] -= mat.m[i][j];
		}
	}
	return*this;
}

//逆行列
Matrix4x4 Matrix4x4::Inverse() const {
	Matrix4x4 result{};
	float determinant = m[0][0] * (m[1][1] * m[2][2] * m[3][3] +
		m[2][1] * m[3][2] * m[1][3] +
		m[3][1] * m[1][2] * m[2][3] -
		m[3][1] * m[2][2] * m[1][3] -
		m[2][1] * m[1][2] * m[3][3] -
		m[1][1] * m[3][2] * m[2][3]) -
		m[0][1] * (m[1][0] * m[2][2] * m[3][3] +
			m[2][0] * m[3][2] * m[1][3] +
			m[3][0] * m[1][2] * m[2][3] -
			m[3][0] * m[2][2] * m[1][3] -
			m[2][0] * m[1][2] * m[3][3] -
			m[1][0] * m[3][2] * m[2][3]) +
		m[0][2] * (m[1][0] * m[2][1] * m[3][3] +
			m[2][0] * m[3][1] * m[1][3] +
			m[3][0] * m[1][1] * m[2][3] -
			m[3][0] * m[2][1] * m[1][3] -
			m[2][0] * m[1][1] * m[3][3] -
			m[1][0] * m[3][1] * m[2][3]) -
		m[0][3] * (m[1][0] * m[2][1] * m[3][2] +
			m[2][0] * m[3][1] * m[1][2] +
			m[3][0] * m[1][1] * m[2][2] -
			m[3][0] * m[2][1] * m[1][2] -
			m[2][0] * m[1][1] * m[3][2] -
			m[1][0] * m[3][1] * m[2][2]);



	if (determinant != 0) {
		result.m[0][0] = (m[1][1] * m[2][2] * m[3][3] +
			m[2][1] * m[3][2] * m[1][3] +
			m[3][1] * m[1][2] * m[2][3] -
			m[3][1] * m[2][2] * m[1][3] -
			m[2][1] * m[1][2] * m[3][3] -
			m[1][1] * m[3][2] * m[2][3]) /
			determinant;

		result.m[0][1] = -(m[0][1] * m[2][2] * m[3][3] +
			m[2][1] * m[3][2] * m[0][3] +
			m[3][1] * m[0][2] * m[2][3] -
			m[3][1] * m[2][2] * m[0][3] -
			m[2][1] * m[0][2] * m[3][3] -
			m[0][1] * m[3][2] * m[2][3]) /
			determinant;

		result.m[0][2] = (m[0][1] * m[1][2] * m[3][3] +
			m[1][1] * m[3][2] * m[0][3] +
			m[3][1] * m[0][2] * m[1][3] -
			m[3][1] * m[1][2] * m[0][3] -
			m[1][1] * m[0][2] * m[3][3] -
			m[0][1] * m[3][2] * m[1][3]) /
			determinant;

		result.m[0][3] = -(m[0][1] * m[1][2] * m[2][3] +
			m[1][1] * m[2][2] * m[0][3] +
			m[2][1] * m[0][2] * m[1][3] -
			m[2][1] * m[1][2] * m[0][3] -
			m[1][1] * m[0][2] * m[2][3] -
			m[0][1] * m[2][2] * m[1][3]) /
			determinant;


		result.m[1][0] = -(m[1][0] * m[2][2] * m[3][3] +
			m[2][0] * m[3][2] * m[1][3] +
			m[3][0] * m[1][2] * m[2][3] -
			m[3][0] * m[2][2] * m[1][3] -
			m[2][0] * m[1][2] * m[3][3] -
			m[1][0] * m[3][2] * m[2][3]) /
			determinant;

		result.m[1][1] = (m[0][0] * m[2][2] * m[3][3] +
			m[2][0] * m[3][2] * m[0][3] +
			m[3][0] * m[0][2] * m[2][3] -
			m[3][0] * m[2][2] * m[0][3] -
			m[2][0] * m[0][2] * m[3][3] -
			m[0][0] * m[3][2] * m[2][3]) /
			determinant;

		result.m[1][2] = -(m[0][0] * m[1][2] * m[3][3] +
			m[1][0] * m[3][2] * m[0][3] +
			m[3][0] * m[0][2] * m[1][3] -
			m[3][0] * m[1][2] * m[0][3] -
			m[1][0] * m[0][2] * m[3][3] -
			m[0][0] * m[3][2] * m[1][3]) /
			determinant;

		result.m[1][3] = (m[0][0] * m[1][2] * m[2][3] +
			m[1][0] * m[2][2] * m[0][3] +
			m[2][0] * m[0][2] * m[1][3] -
			m[2][0] * m[1][2] * m[0][3] -
			m[1][0] * m[0][2] * m[2][3] -
			m[0][0] * m[2][2] * m[1][3]) /
			determinant;


		result.m[2][0] = (m[1][0] * m[2][1] * m[3][3] +
			m[2][0] * m[3][1] * m[1][3] +
			m[3][0] * m[1][1] * m[2][3] -
			m[3][0] * m[2][1] * m[1][3] -
			m[2][0] * m[1][1] * m[3][3] -
			m[1][0] * m[3][1] * m[2][3]) /
			determinant;

		result.m[2][1] = -(m[0][0] * m[2][1] * m[3][3] +
			m[2][0] * m[3][1] * m[0][3] +
			m[3][0] * m[0][1] * m[2][3] -
			m[3][0] * m[2][1] * m[0][3] -
			m[2][0] * m[0][1] * m[3][3] -
			m[0][0] * m[3][1] * m[2][3]) /
			determinant;

		result.m[2][2] = (m[0][0] * m[1][1] * m[3][3] +
			m[1][0] * m[3][1] * m[0][3] +
			m[3][0] * m[0][1] * m[1][3] -
			m[3][0] * m[1][1] * m[0][3] -
			m[1][0] * m[0][1] * m[3][3] -
			m[0][0] * m[3][1] * m[1][3]) /
			determinant;

		result.m[2][3] = -(m[0][0] * m[1][1] * m[2][3] +
			m[1][0] * m[2][1] * m[0][3] +
			m[2][0] * m[0][1] * m[1][3] -
			m[2][0] * m[1][1] * m[0][3] -
			m[1][0] * m[0][1] * m[2][3] -
			m[0][0] * m[2][1] * m[1][3]) /
			determinant;

		result.m[3][0] = -(m[1][0] * m[2][1] * m[3][2] +
			m[2][0] * m[3][1] * m[1][2] +
			m[3][0] * m[1][1] * m[2][2] -
			m[3][0] * m[2][1] * m[1][2] -
			m[2][0] * m[1][1] * m[3][2] -
			m[1][0] * m[3][1] * m[2][2]) /
			determinant;

		result.m[3][1] = (m[0][0] * m[2][1] * m[3][2] +
			m[2][0] * m[3][1] * m[0][2] +
			m[3][0] * m[0][1] * m[2][2] -
			m[3][0] * m[2][1] * m[0][2] -
			m[2][0] * m[0][1] * m[3][2] -
			m[0][0] * m[3][1] * m[2][2]) /
			determinant;

		result.m[3][2] = -(m[0][0] * m[1][1] * m[3][2] +
			m[1][0] * m[3][1] * m[0][2] +
			m[3][0] * m[0][1] * m[1][2] -
			m[3][0] * m[1][1] * m[0][2] -
			m[1][0] * m[0][1] * m[3][2] -
			m[0][0] * m[3][1] * m[1][2]) /
			determinant;

		result.m[3][3] = (m[0][0] * m[1][1] * m[2][2] +
			m[1][0] * m[2][1] * m[0][2] +
			m[2][0] * m[0][1] * m[1][2] -
			m[2][0] * m[1][1] * m[0][2] -
			m[1][0] * m[0][1] * m[2][2] -
			m[0][0] * m[2][1] * m[1][2]) /
			determinant;
	}

	return result;
}

//転置行列
Matrix4x4 Matrix4x4::Transpose() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = this->m[j][i];
		}
	}
	return result;
}

//逆転置行列
Matrix4x4 Matrix4x4::InverseTranspose() {
	Matrix4x4 result = this->Inverse();
	return result.Transpose();
}

//単位行列
Matrix4x4 Matrix4x4::Identity4x4() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1;
	}
	return result;
}
