#include "Quaternion.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include <cmath>
#include <algorithm>

//乗法単位元
Quaternion Quaternion::IdentityQuaternion(){
	return { 0.0f,0.0f,0.0f,1.0f };
}

//共役
Quaternion Quaternion::Conjugate()const{
	Quaternion result = *this;
	result.x *= -1.0f;
	result.y *= -1.0f;
	result.z *= -1.0f;
	return result;
}

//内積
float Quaternion::Dot(const Quaternion& q) const{
	return { x * q.x + y * q.y + z * q.z + w * q.w };
}

//長さ(ノルム)
float Quaternion::Length()const{
	return std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f) + std::pow(z, 2.0f) + std::pow(w, 2.0f));
}

//逆クォータニオン
Quaternion Quaternion::Inverse()const{
	//共役
	Quaternion conjugate = this->Conjugate();
	//ノルム
	float norm = this->Length();
	//逆Quaternion
	Quaternion inverse = conjugate / std::pow(norm, 2.0f);
	return inverse;
}

//単位クォータニオン
Quaternion Quaternion::Normalize()const{
	Quaternion normalize = *this;
	float norm = this->Length();
	if (norm != 0.0f){
		normalize = normalize / norm;
	}
	return normalize;
}

//球面線形補間
Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t){
	Quaternion result = {};

	//クォータニオンの正規化
	Quaternion begin = q1.Normalize();
	Quaternion end = q2.Normalize();

	//内積
	float dot = begin.Dot(end);

	//最短距離を求める
	if (dot < 0.0f){
		begin = -begin;
		dot = -dot;
	}

	//浮動小数の誤差でacosの範囲を超えないようにする
	dot = std::clamp(dot, 0.0f, 1.0f);

	//ほぼ同じ向きなら線形補完する
	if (dot > 0.9995f){
		result = (1.0f - t) * begin + t * end;

		return result.Normalize();
	}

	//acosでθを求める
	float theta = std::acos(dot);

	//球面線形補間
	result = std::sin((1.0f - t) * theta) / std::sin(theta) * begin + std::sin(t * theta) / std::sin(theta) * end;

	return result.Normalize();
}

//オイラー角からクォータニオンを生成
Quaternion Quaternion::EulerAngleToQuaternion(const Vector3& rotate){
	float rx = rotate.x;
	float ry = rotate.y;
	float rz = rotate.z;

	Quaternion qx = { std::sin(rx * 0.5f), 0.0f, 0.0f, std::cos(rx * 0.5f) };
	Quaternion qy = { 0.0f, std::sin(ry * 0.5f), 0.0f, std::cos(ry * 0.5f) };
	Quaternion qz = { 0.0f, 0.0f, std::sin(rz * 0.5f), std::cos(rz * 0.5f) };

	// 掛ける順番は座標系や実装方針で変わる
	Quaternion q = qz * qy * qx;
	q = q.Normalize();
	return q;
}

//ベクトルをクォータニオンで回転させた結果のベクトルを求める
Vector3 Quaternion::RotateVector(const Vector3& vector)const{
	Quaternion result = Quaternion::IdentityQuaternion();
	Quaternion q = (*this).Normalize();
	Quaternion r = { vector.x,vector.y,vector.z,0.0f };
	result = q * r * q.Conjugate();
	return { result.x,result.y,result.z };
}

//任意軸回転を表すクォータニオンの生成
Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle){
	Quaternion result = Quaternion::IdentityQuaternion();
	//cos
	float cos = std::cos(angle / 2.0f);
	//sin
	float sin = std::sin(angle / 2.0f);

	//3軸を正規化
	Vector3 n = axis.Normalize();

	return { n.x * sin,n.y * sin,n.z * sin,cos };
}

//行列をクォータニオンに変換
Quaternion Quaternion::RotationMatrixToQuaternion(const Matrix4x4& m){
	//トレースを作成
	float trace = m.m[0][0] + m.m[1][1] + m.m[2][2];

	Quaternion result = Quaternion::IdentityQuaternion();
	//トレースを見る
	if (trace > 0.0f){
		//トレースを基準
		float s = std::sqrt(trace + 1.0f) * 2.0f;

		//クォータニオンに変換
		result.x = (m.m[1][2] - m.m[2][1]) / s;
		result.y = (m.m[2][0] - m.m[0][2]) / s;
		result.z = (m.m[0][1] - m.m[1][0]) / s;
		result.w = s / 4.0f;
	} else if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]){
		//xを基準
		float s = std::sqrt(1.0f + m.m[0][0] - m.m[1][1] - m.m[2][2]) * 2.0f;

		//クォータニオンに変換
		result.x = s / 4.0f;
		result.y = (m.m[0][1] + m.m[1][0]) / s;
		result.z = (m.m[0][2] + m.m[2][0]) / s;
		result.w = (m.m[1][2] - m.m[2][1]) / s;
	} else if (m.m[1][1] > m.m[2][2]){
		//yを基準
		float s = std::sqrt(1.0f + m.m[1][1] - m.m[0][0] - m.m[2][2]) * 2.0f;


		//クォータニオンに変換
		result.x = (m.m[0][1] + m.m[1][0]) / s;
		result.y = s / 4.0f;
		result.z = (m.m[1][2] + m.m[2][1]) / s;
		result.w = (m.m[2][0] - m.m[0][2]) / s;
	} else{
		//zを基準
		float s = std::sqrt(1.0f + m.m[2][2] - m.m[0][0] - m.m[1][1]) * 2.0f;

		//クォータニオンに変換
		result.x = (m.m[0][2] + m.m[2][0]) / s;
		result.y = (m.m[1][2] + m.m[2][1]) / s;
		result.z = s / 4.0f;
		result.w = (m.m[0][1] - m.m[1][0]) / s;
	}

	return result.Normalize();
}

//加算
Quaternion Quaternion::operator+(const Quaternion& q) const{
	Quaternion result = {};
	result.x = x + q.x;
	result.y = y + q.y;
	result.z = z + q.z;
	result.w = w + q.w;
	return result;
}

//負の数にする
Quaternion Quaternion::operator-() const{
	Quaternion result = {};
	result.x = -x;
	result.y = -y;
	result.z = -z;
	result.w = -w;
	return result;
}

//乗法
Quaternion Quaternion::operator*(const Quaternion& q)const{
	//クォータニオン
	Quaternion r = q;

	//虚部(ベクトル部)
	Vector3 qv = Vector3(x, y, z);
	Vector3 rv = Vector3(r.x, r.y, r.z);

	//返す値
	Vector3 resultV = {};
	float resultW = 0.0f;
	resultV = qv.Cross(rv) + qv * r.w + rv * w;
	resultW = w * r.w - qv.Dot(rv);

	return { resultV.x,resultV.y,resultV.z,resultW };
}

//除算(float)
Quaternion Quaternion::operator/(float num)const{
	Quaternion result;
	result.x = x / num;
	result.y = y / num;
	result.z = z / num;
	result.w = w / num;
	return result;
}

//浮動小数 * クォータニオン
Quaternion operator*(float num, const Quaternion& q){
	Quaternion result = {};
	result.x = q.x * num;
	result.y = q.y * num;
	result.z = q.z * num;
	result.w = q.w * num;
	return result;
}
