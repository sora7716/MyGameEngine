#define NOMINMAX
#include "MathUtility.h"
#include <cassert>
using namespace std;

// トランスフォームノーマル
Vector3 mathUtility::TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};
	return result;
}

// 線形補間
Vector3 mathUtility::Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result;
	result.x = v1.x + t * (v2.x - v1.x);
	result.y = v1.y + t * (v2.y - v1.y);
	result.z = v1.z + t * (v2.z - v1.z);

	return result;
}

// 球面線形補間
Vector3 mathUtility::Slerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 nv1 = v1; // v1 の正規化ベクトル
	nv1 = nv1.Normalize();
	Vector3 nv2 = v2; // v2 の正規化ベクトル
	nv2 = nv2.Normalize();
	float dot = nv1.Dot(nv2);   // 正規化されたベクトル同士の内積

	// 誤差により1.0fを超えるのを防ぐ
	dot = std::clamp(dot, -1.0f, 1.0f);

	// アークコサインでθの角度を求める
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);

	Vector3 normalizeVector;
	// ゼロ除算を防ぐ
	if (sinTheta < 1.0e-5) {
		normalizeVector = nv1;
	} else {
		// 球面線形補間したベクトル(単位ベクトル)
		float sinThetaFrom = std::sin((1 - t) * theta);
		float sinThetaTo = std::sin(t * theta);
		normalizeVector = (sinThetaFrom * nv1 + sinThetaTo * nv2) / sinTheta;
	}

	// ベクトルの長さはv1とv2の長さを線形補間
	Vector3 tempV1 = v1;
	Vector3 tempV2 = v2;
	float length1 = tempV1.Length();
	float length2 = tempV2.Length();
	// Lerpで補間ベクトルの長さを求める
	float length = std::lerp(length1, length2, t);

	return normalizeVector * length;
}

// CatmullRom補間
Vector3 mathUtility::CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	const float s = 0.5f; // 1/2のこと

	float t2 = t * t;  // tの2乗
	float t3 = t2 * t; // tの3乗

	Vector3 e3 = p0 * -1 + 3 * p1 - 3 * p2 + p3;
	Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
	Vector3 e1 = p0 * -1 + p2;
	Vector3 e0 = 2 * p1;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

// CatmullRomスプライン曲線上の座標を得る
Vector3 mathUtility::CatmullRomPosition(const std::vector<Vector3>& points, float t) {
	assert(points.size() >= 4 && "制御点は4点以上必要です");
	// 区間数は制御点の数-1
	size_t division = points.size() - 1;
	// 1区間の長さ(全体を1.0とした割合)
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f、終点1.0fとしたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限(0.0f)とz上限(1.0f)の範囲を収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);
	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないように収める
	index = std::min(index, division - 1);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区間のp0はp1を重複使用する
	if (index == 0) {
		index0 = index1;
	}

	// 最後の区間のp3はp2を重複使用する
	if (index3 >= points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 4点を指定してCatmull-Rom補間
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}

// 三次元のベジエ曲線
Vector3 mathUtility::Bezier(const Vector3* points, float t) {
	//制御点を分ける
	Vector3 p0 = points[0];//始点
	Vector3 p1 = points[1];//中点
	Vector3 p2 = points[2];//終点

	Vector3 p0p1 = Lerp(p0, p1, t);  // p0とp1の間を補間
	Vector3 p1p2 = Lerp(p1, p2, t);  // p1とp2の間を補間
	Vector3 p = Lerp(p0p1, p1p2, t); // 上記2つの補間結果をさらに補間
	return p;
}

// 三次元ベジエ曲線(球面線形補間ver)
Vector3 mathUtility::BezierS(const Vector3* points, float t) {
	//制御点を分ける
	Vector3 p0 = points[0];//始点
	Vector3 p1 = points[1];//中点
	Vector3 p2 = points[2];//終点

	Vector3 p0p1 = Slerp(p0, p1, t);  // p0とp1の間を補間
	Vector3 p1p2 = Slerp(p1, p2, t);  // p1とp2の間を補間
	Vector3 p = Slerp(p0p1, p1p2, t); // 上記2つの補間結果をさらに補間
	return p;
}

// リサージュ曲線
Vector3 mathUtility::LissajousCurve(const Vector3& theta, const Vector3& center, const Vector3& scalar) {
	Vector3 result{};
	result.x = scalar.x * sin(theta.x) + center.x;
	result.y = scalar.y * sin(theta.y) + center.y;
	result.z = scalar.z * sin(theta.z) + center.z;
	return result;
}

//逆正接関数のcotangent
float mathUtility::Cont(float theta) {
	return (1.0f / tanf(theta));
}

// 円運動XZ
Vector3 mathUtility::CircularMoveXZ(const Vector3& center, const Vector2& radius, float theta) {
	Vector3 result{};
	//円運動させる
	result.x = center.x + cos(theta) * radius.x;
	result.y = center.y;
	result.z = center.z + sin(theta) * radius.y;
	return result;
}

//平面の作成(無限平面)
primitiveData::Plane mathUtility::MakePlane(const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	//平面
	primitiveData::Plane plane = {};

	//p0　-> p1へ向かうベクトル
	Vector3 v01 = p1 - p0;
	//p0　-> p2へ向かうベクトル
	Vector3 v02 = p2 - p0;
	//法線
	plane.normal = v01.Cross(v02).Normalize();

	//距離
	plane.distance = plane.normal.Dot(p0);

	return plane;
}

//視錐台の頂点の作成
std::array<Vector3, 8> mathUtility::CreateFrustumVertex(float nearClip, float farClip, float fovY, float aspect) {
	std::array<Vector3, 8>vertexes = {};
	float nearZ = nearClip;
	float farZ = farClip;

	float nearH = std::tan(fovY * 0.5f) * nearZ;
	float nearW = nearH * aspect;

	float farH = std::tan(fovY * 0.5f) * farZ;
	float farW = farH * aspect;

	//near
	vertexes[0] = { -nearW,-nearH,nearZ };
	vertexes[1] = { -nearW,nearH,nearZ };
	vertexes[2] = { nearW,nearH,nearZ };
	vertexes[3] = { nearW,-nearH,nearZ };

	//far
	vertexes[4] = { -farW,-farH,farZ };
	vertexes[5] = { -farW,farH,farZ };
	vertexes[6] = { farW,farH,farZ };
	vertexes[7] = { farW,-farH,farZ };

	return vertexes;
}

//視錐台の作成
primitiveData::Frustum mathUtility::CreateFrustumData(const std::array<Vector3, 8>& vertices, const Matrix4x4& worldMatrix) {
	primitiveData::Frustum frustum = {};
	frustum.localCorners = vertices;
	for (uint32_t i = 0; i < frustum.worldCorners.size(); i++) {
		Vector3 local = vertices[i];

		Vector3 world = local * worldMatrix;

		frustum.worldCorners[i] = world;
	}

	const auto& c = frustum.worldCorners;

	frustum.planes[primitiveData::Frustum::kLeft] = mathUtility::MakePlane(c[0], c[1], c[4]);

	frustum.planes[primitiveData::Frustum::kRight] = mathUtility::MakePlane(c[3], c[7], c[2]);

	frustum.planes[primitiveData::Frustum::kTop] = mathUtility::MakePlane(c[1], c[2], c[5]);

	frustum.planes[primitiveData::Frustum::kBottom] = mathUtility::MakePlane(c[0], c[4], c[3]);

	frustum.planes[primitiveData::Frustum::kNear] = mathUtility::MakePlane(c[0], c[2], c[1]);

	frustum.planes[primitiveData::Frustum::kFar] = mathUtility::MakePlane(c[4], c[5], c[6]);

	return frustum;
}

//平行四辺形の面積を求める
float mathUtility::CalcParallelogramArea(const std::array<Vector3, 3>& vertices) {
	//各頂点を取得
	Vector3 v0 = vertices[0];
	Vector3 v1 = vertices[1];
	Vector3 v2 = vertices[2];

	//v1-v0ベクトル
	Vector3 v01 = v1 - v0;//底辺の長さベクトル
	//v2-v0ベクトル
	Vector3 v02 = v2 - v0;//高さ

	//底辺の長さ x 高さ
	Vector3 cross = v01.Cross(v02);
	float crossLength = cross.Length();

	return crossLength;
}

//三角形の面積を求める
float mathUtility::CalcTriangleArea(const std::array<Vector3, 3>& vertices) {
	return CalcParallelogramArea(vertices) / 2.0f;
}

//平行四辺形の面積を処理を早くして(正確じゃない)
float mathUtility::CalcParallelogramAreaSquared(const std::array<Vector3, 3>& vertices) {
	//三角形の面積
	Vector3 v0 = vertices[0];
	Vector3 v1 = vertices[1];
	Vector3 v2 = vertices[2];

	//v1-v0ベクトル
	Vector3 v01 = v1 - v0;//底辺の長さベクトル
	//v2-v0ベクトル
	Vector3 v02 = v2 - v0;//高さ

	//底辺の長さ x 高さ
	Vector3 cross = v01.Cross(v02);
	float crossLength = cross.LengthSquared();

	return crossLength;
}

//クォータニオンからオイラー角を求める
Vector3 mathUtility::MakeEulerAngleForQuaternion(const Quaternion& quaternion) {
	Quaternion q = quaternion.Normalize();
	Vector3 eulerAngle = {};

	//X軸回転
	float sinX = 2.0f * (q.w * q.x + q.y * q.z);
	float cosX = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	eulerAngle.x = std::atan2(sinX, cosX);
	//Y軸回転
	float sinY = 2.0f * (q.w * q.y - q.z * q.x);
	sinY = std::clamp(sinY, -1.0f, 1.0f);
	eulerAngle.y = std::asin(sinY);
	//Z軸回転
	float sinZ = 2.0f * (q.w * q.z + q.x * q.y);
	float cosZ = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	eulerAngle.z = std::atan2(sinZ, cosZ);

	return eulerAngle;
}
