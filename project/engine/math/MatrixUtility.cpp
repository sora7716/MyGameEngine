#include "MatrixUtility.h"
#include "MathUtility.h"
#include "Logger.h"
#include "StringUtility.h"
#include <cassert>
using namespace std;

//拡縮
Matrix4x4 MatrixUtility::MakeScaleMatrix(const Vector3& scale) {
	//単位行列で初期化
	Matrix4x4 result = Matrix4x4::Identity4x4();
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}

//平行移動
Matrix4x4 MatrixUtility::MakeTranslateMatrix(const Vector3& translate) {
	//単位行列で初期化
	Matrix4x4 result = Matrix4x4::Identity4x4();
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

//x座標を軸に回転
Matrix4x4 MatrixUtility::MakeRotateXMatrix(const float& radian) {
	//単位行列で初期化
	Matrix4x4 result = Matrix4x4::Identity4x4();
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	return result;
}

//y座標を軸に回転
Matrix4x4 MatrixUtility::MakeRotateYMatrix(const float& radian) {
	//単位行列で初期化
	Matrix4x4 result = Matrix4x4::Identity4x4();
	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);
	return result;
}

//z座標を軸に回転
Matrix4x4 MatrixUtility::MakeRotateZMatrix(const float& radian) {
	//単位行列で初期化
	Matrix4x4 result = Matrix4x4::Identity4x4();
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	return result;
}

//x,y,z座標で回転
Matrix4x4 MatrixUtility::MakeRotateMatrix(const Vector3& radian) {
	return MakeRotateXMatrix(radian.x) * MakeRotateYMatrix(radian.y) * MakeRotateZMatrix(radian.z);
}

//任意軸回転
Matrix4x4 MatrixUtility::MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Matrix4x4 result = Matrix4x4::Identity4x4();
	//単位ベクトル
	Vector3 n = axis.Normalize();
	//cos
	float cos = std::cos(angle);
	//sin
	float sin = std::sin(angle);

	result.m[0][0] = std::pow(n.x, 2.0f) * (1.0f - cos) + cos;
	result.m[0][1] = n.x * n.y * (1.0f - cos) + n.z * sin;
	result.m[0][2] = n.x * n.z * (1.0f - cos) - n.y * sin;
	result.m[1][0] = n.x * n.y * (1.0f - cos) - n.z * sin;
	result.m[1][1] = std::pow(n.y, 2.0f) * (1.0f - cos) + cos;
	result.m[1][2] = n.y * n.z * (1.0f - cos) + n.x * sin;
	result.m[2][0] = n.x * n.z * (1 - cos) + n.y * sin;
	result.m[2][1] = n.y * n.z * (1.0f - cos) - n.x * sin;
	result.m[2][2] = std::pow(n.z, 2.0f) * (1.0f - cos) + cos;

	return result;
}

//fromからtoの方向へ向く回転行列
Matrix4x4 MatrixUtility::DirectionToDirection(const Vector3& from, const Vector3& to) {
	Matrix4x4 result = Matrix4x4::Identity4x4();
	Vector3 u = from.Normalize();
	Vector3 v = to.Normalize();
	Vector3 n = (v.Cross(u)).Normalize();
	float cosTheta = u.Dot(v);
	float sinTheta = u.Cross(v).Length();

	if (cosTheta <= -1.0f) {
		if (u.x != 0.0f || u.y != 0.0f) {
			n = Vector3(u.y, -u.x, 0.0f).Normalize();
		} else if (u.x != 0.0f || u.z != 0.0f) {
			n = Vector3(u.z, 0.0f, -u.x).Normalize();
		}
	}

	result.m[0][0] = std::pow(n.x, 2.0f) * (1.0f - cosTheta) + cosTheta;
	result.m[0][1] = n.x * n.y * (1.0f - cosTheta) + n.z * sinTheta;
	result.m[0][2] = n.x * n.z * (1.0f - cosTheta) - n.y * sinTheta;

	result.m[1][0] = n.x * n.y * (1.0f - cosTheta) - n.z * sinTheta;
	result.m[1][1] = std::pow(n.y, 2.0f) * (1.0f - cosTheta) + cosTheta;
	result.m[1][2] = n.y * n.z * (1.0f - cosTheta) + n.x * sinTheta;

	result.m[2][0] = n.x * n.z * (1.0f - cosTheta) + n.y * sinTheta;
	result.m[2][1] = n.y * n.z * (1.0f - cosTheta) - n.x * sinTheta;
	result.m[2][2] = std::pow(n.z, 2.0f) * (1.0f - cosTheta) + cosTheta;
	return result;
}

//任意軸回転を表すクォータニオンの生成
Quaternion MatrixUtility::MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Quaternion result = Quaternion::IdentityQuaternion();
	//cos
	float cos = std::cos(angle / 2.0f);
	//sin
	float sin = std::sin(angle / 2.0f);

	//3軸を正規化
	Vector3 n = axis.Normalize();

	return { n.x * sin,n.y * sin,n.z * sin,cos };
}

//ベクトルをクォータニオンで回転させた結果のベクトルを求める
Vector3 MatrixUtility::RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Quaternion result = Quaternion::IdentityQuaternion();
	Quaternion q = quaternion.Normalize();
	Quaternion r = { vector.x,vector.y,vector.z,0.0f };
	result = q * r * q.Conjugate();
	return { result.x,result.y,result.z };
}

//Quaternionから回転行列を求める
Matrix4x4 MatrixUtility::MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result = Matrix4x4::Identity4x4();
	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;
	result.m[0][0] = std::pow(w, 2.0f) + std::pow(x, 2.0f) - std::pow(y, 2.0f) - std::pow(z, 2.0f);
	result.m[0][1] = 2.0f * (x * y + w * z);
	result.m[0][2] = 2.0f * (x * z - w * y);
	result.m[1][0] = 2.0f * (x * y - w * z);
	result.m[1][1] = std::pow(w, 2.0f) - std::pow(x, 2.0f) + std::pow(y, 2.0f) - std::pow(z, 2.0f);
	result.m[1][2] = 2.0f * (y * z + w * x);
	result.m[2][0] = 2.0f * (x * z + w * y);
	result.m[2][1] = 2.0f * (y * z - w * x);
	result.m[2][2] = std::pow(w, 2.0f) - std::pow(x, 2.0f) - std::pow(y, 2.0f) + std::pow(z, 2.0f);

	return result;
}

// OBB用の回転行列
void MatrixUtility::MakeOBBRotateMatrix(Vector3* orientations, const Quaternion& rotate) {
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);

	//回転行列からの抽出

	//X'
	orientations[0].x = rotateMatrix.m[0][0];
	orientations[0].y = rotateMatrix.m[0][1];
	orientations[0].z = rotateMatrix.m[0][2];

	//Y'
	orientations[1].x = rotateMatrix.m[1][0];
	orientations[1].y = rotateMatrix.m[1][1];
	orientations[1].z = rotateMatrix.m[1][2];

	//Z'
	orientations[2].x = rotateMatrix.m[2][0];
	orientations[2].y = rotateMatrix.m[2][1];
	orientations[2].z = rotateMatrix.m[2][2];
}

// OBB用のワールド行列
Matrix4x4 MatrixUtility::MakeOBBWorldMatrix(const Vector3* orientations, const Vector3 center) {
	Matrix4x4 result{
		orientations[0].x,orientations[0].y,orientations[0].z,0.0f,
		orientations[1].x,orientations[1].y,orientations[1].z,0.0f,
		orientations[2].x,orientations[2].y,orientations[2].z,0.0f,
		center.x,center.y,center.z,1.0f,
	};
	return result;
}

//アフィン関数
Matrix4x4 MatrixUtility::MakeAffineMatrix(const Transform& transform) {
	//Quaternion q = MakeRotateQuaternion(transform.quaternion);
	return (MakeScaleMatrix(transform.scale) * MakeRotateMatrix(transform.quaternion)) * MakeTranslateMatrix(transform.translate);
}

//アフィン行列
Matrix4x4 MatrixUtility::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	return (MakeScaleMatrix(scale) * MakeRotateMatrix(rotate)) * MakeTranslateMatrix(translate);
}

//STRの変換
Matrix4x4 MatrixUtility::MakeSTRMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	return MakeScaleMatrix(scale) * MakeTranslateMatrix(translate) * MakeRotateMatrix(rotate);
}

// UVのアフィン変換
Matrix4x4 MatrixUtility::MakeUVAffineMatrix(const Transform2d& uvTransform) {
	return MakeScaleMatrix({ uvTransform.scale.x,uvTransform.scale.y,1.0f }) * MakeRotateZMatrix(uvTransform.rotate) * MakeTranslateMatrix({ uvTransform.translate.x,uvTransform.translate.y,1.0f });
}

// 正射影行列
Matrix4x4 MatrixUtility::MakeOrthographicMatrix(const float& left, const float& top, const float& right, const float& bottom, const float& nearClip, const float& farClip) {
	Matrix4x4 result{
		2.0f / (right - left),0.0f,0.0f,0.0f,
		0.0f,2.0f / (top - bottom),0.0f,0.0f,
		0.0f,0.0f,1.0f / (farClip - nearClip),0.0f,
		(left + right) / (left - right),(top + bottom) / (bottom - top),nearClip / (nearClip - farClip),1.0f
	};

	return result;
}

//透視投影行列
Matrix4x4 MatrixUtility::MakePerspectiveFovMatrix(const float& fovY, const float& aspectRation, const float& nearClip, const float& farClip) {
	Matrix4x4 result{
		1.0f / aspectRation * MathUtility::Cont(fovY / 2.0f),0.0f,0.0f,0.0f,
		0.0f, MathUtility::Cont(fovY / 2.0f),0.0f,0.0f,
		0.0f,0.0f,farClip / (farClip - nearClip),1.0f,
		0.0f,0.0f,-(nearClip * farClip) / (farClip - nearClip),0.0f
	};

	return result;
}

//ビューポートマトリックス
Matrix4x4 MatrixUtility::MakeViewportMatrix(const float& left, const float& top, const float& width, const float& height, const float& minDepth, const float& maxDepth) {
	Matrix4x4 result{
		width / 2.0f,0.0f,0.0f,0.0f,
		0.0f,-height / 2.0f,0.0f,0.0f,
		0.0f,0.0f,maxDepth - minDepth,0.0f,
		(left + width) / 2.0f,(top + height) / 2.0f,minDepth,1.0f,
	};
	return result;
}

//ビルボード行列を作成
Matrix4x4 MatrixUtility::MakeBillboardMatrix(const Matrix4x4& cameraWorldMatrix, const Vector3& rotate) {
	//正面に向けるY軸回転の行列を作成
	Matrix4x4 backToFrontMatrix = MatrixUtility::MakeRotateMatrix(rotate);
	//ビルボード行列を作成
	Matrix4x4 billboardMatrix = backToFrontMatrix * cameraWorldMatrix;
	billboardMatrix.m[3][0] = 0.0f; // X座標を0に設定
	billboardMatrix.m[3][1] = 0.0f; // Y座標を0に設定
	billboardMatrix.m[3][2] = 0.0f; // Z座標を0に設定
	return billboardMatrix;
}

//ビルボード行列を作成
Matrix4x4 MatrixUtility::MakeBillboardMatrix(const Matrix4x4& cameraWorldMatrix, const Quaternion& quaternion) {
	//正面に向けるY軸回転の行列を作成
	Matrix4x4 backToFrontMatrix = MatrixUtility::MakeRotateMatrix(quaternion);
	//ビルボード行列を作成
	Matrix4x4 billboardMatrix = backToFrontMatrix * cameraWorldMatrix;
	billboardMatrix.m[3][0] = 0.0f; // X座標を0に設定
	billboardMatrix.m[3][1] = 0.0f; // Y座標を0に設定
	billboardMatrix.m[3][2] = 0.0f; // Z座標を0に設定
	return billboardMatrix;
}

//ビルボード行列を含んだアフィン行列の作成
Matrix4x4 MatrixUtility::MakeBillboardAffineMatrix(const Matrix4x4& cameraWorldMatrix, const Transform& transform) {
	return (MakeScaleMatrix(transform.scale) * MakeBillboardMatrix(cameraWorldMatrix, transform.quaternion)) * MakeTranslateMatrix(transform.translate);
}

//行列をTransformDataに分解
Transform MatrixUtility::DecomposeMatrix(const Matrix4x4& mat) {
	Transform result{};
	//拡縮
	result.scale.x = std::sqrt(
		std::pow(mat.m[0][0], 2.0f) +
		std::pow(mat.m[0][1], 2.0f) +
		std::pow(mat.m[0][2], 2.0f)
	);
	result.scale.y = std::sqrt(
		std::pow(mat.m[1][0], 2.0f) +
		std::pow(mat.m[1][1], 2.0f) +
		std::pow(mat.m[1][2], 2.0f)
	);
	result.scale.z = std::sqrt(
		std::pow(mat.m[2][0], 2.0f) +
		std::pow(mat.m[2][1], 2.0f) +
		std::pow(mat.m[2][2], 2.0f)
	);

	//回転
	// 回転行列の正規化（スケール除去）
	float rm00 = mat.m[0][0] / result.scale.x;
	float rm01 = mat.m[0][1] / result.scale.x;
	float rm02 = mat.m[0][2] / result.scale.x;

	float rm10 = mat.m[1][0] / result.scale.y;
	float rm11 = mat.m[1][1] / result.scale.y;
	float rm12 = mat.m[1][2] / result.scale.y;

	float rm20 = mat.m[2][0] / result.scale.z;
	float rm21 = mat.m[2][1] / result.scale.z;
	float rm22 = mat.m[2][2] / result.scale.z;

	// オイラー角 (Yaw-Pitch-Roll 順)
	result.quaternion.x = std::atan2(-rm21, sqrtf(rm20 * rm20 + rm22 * rm22));//Pitch
	result.quaternion.y = std::atan2(rm20, rm22);//Yaw
	result.quaternion.z = std::atan2(rm01, rm11);//Roll

	//平行移動
	result.translate = {
		mat.m[3][0],
		mat.m[3][1],
		mat.m[3][2]
	};
	return result;
}
