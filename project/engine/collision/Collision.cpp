#define NOMINMAX
#include "Collision.h"
#include "MatrixUtility.h"
#include "Logger.h"
#include <format>
#include <algorithm>
#include <array>
#include <cmath>

using namespace primitiveData;

/// <summary>
/// 球同士の衝突判定
/// </summary>
/// <param name="sphere1">球1</param>
/// <param name="sphere2">球2</param>
/// <returns>衝突したかどうか</returns>
bool collision::IsCollision(const Sphere& sphere1, const Sphere& sphere2) {
	//距離
	float distance = (sphere2.center - sphere1.center).Length();
	//距離と二つの球の半径を足した値と比べて
	return distance < (sphere1.radius + sphere2.radius);
}

/// <summary>
/// AABB同士の衝突判定
/// </summary>
/// <param name="aabb1">aabb1</param>
/// <param name="aabb2">aabb2</param>
/// <returns>衝突したかのフラグ</returns>
bool collision::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	return aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x &&
		aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y &&
		aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z;
}

/// <summary>
/// AABBと球の衝突判定
/// </summary>
/// <param name="aabb">aabb</param>
/// <param name="sphere">球</param>
/// <returns>衝突したかのフラグ</returns>
bool collision::IsCollision(const AABB& aabb, const Sphere& sphere) {
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};

	//最近接点から球の中心との距離を求める
	float distance = (closestPoint - sphere.center).Length();

	//距離が半径より小さければ衝突
	return distance <= sphere.radius;
}

/// <summary>
/// OBBと球の衝突判定
/// </summary>
/// <param name="obb">obb</param>
/// <param name="sphere">球</param>
/// <returns>衝突したかのフラグ</returns>
bool collision::IsCollision(const OBB& obb, const Sphere& sphere) {
	//OBBのワールド行列の逆行列を作成
	Matrix4x4 obbWorldMatrixInverse = matrixUtility::MakeOBBWorldMatrix(obb.orientations, obb.center).Inverse();

	//球の中心をOBB空間上に持っていく
	Vector3 centerInOBBLocalSpace = sphere.center * obbWorldMatrixInverse;

	//AABBをOBBのサイズをもとに作成
	AABB aabbOBBLocal = { -obb.size,obb.size };

	//OBB空間上に持って行った中心座標を元に球を作成
	Sphere sphereOBBLocal = { centerInOBBLocalSpace,sphere.radius };

	//AABBと球の衝突判定をする
	return collision::IsCollision(aabbOBBLocal, sphereOBBLocal);
}

// OBBとAABB/OBB
bool collision::IsCollision(const OBB& obb1, const OBB& obb2) {
	//分離軸
	std::array<Vector3, 15> separationAxes;

	//面の法線
	//obb1
	separationAxes[0] = obb1.orientations[0];
	separationAxes[1] = obb1.orientations[1];
	separationAxes[2] = obb1.orientations[2];

	//obb2
	separationAxes[3] = obb2.orientations[0];
	separationAxes[4] = obb2.orientations[1];
	separationAxes[5] = obb2.orientations[2];

	//各辺のクロス積
	separationAxes[6] = obb1.orientations[0].Cross(obb2.orientations[0]);
	separationAxes[7] = obb1.orientations[0].Cross(obb2.orientations[1]);
	separationAxes[8] = obb1.orientations[0].Cross(obb2.orientations[2]);

	separationAxes[9] = obb1.orientations[1].Cross(obb2.orientations[0]);
	separationAxes[10] = obb1.orientations[1].Cross(obb2.orientations[1]);
	separationAxes[11] = obb1.orientations[1].Cross(obb2.orientations[2]);

	separationAxes[12] = obb1.orientations[2].Cross(obb2.orientations[0]);
	separationAxes[13] = obb1.orientations[2].Cross(obb2.orientations[1]);
	separationAxes[14] = obb1.orientations[2].Cross(obb2.orientations[2]);

	//obb1の各頂点
	std::array<Vector3, 8> obb1Vertices;
	//obb1のセンターからの長さ
	std::array<Vector3, 3>obb1Length = {
		obb1.orientations[0] * obb1.size.x,
		obb1.orientations[1] * obb1.size.y,
		obb1.orientations[2] * obb1.size.z
	};
	//背面
	//左上
	obb1Vertices[0] = obb1.center - obb1Length[0] + obb1Length[1] + obb1Length[2];
	//左下
	obb1Vertices[1] = obb1.center - obb1Length[0] - obb1Length[1] + obb1Length[2];
	//右上
	obb1Vertices[2] = obb1.center + obb1Length[0] + obb1Length[1] + obb1Length[2];
	//右下
	obb1Vertices[3] = obb1.center + obb1Length[0] - obb1Length[1] + obb1Length[2];

	//正面
	//左上
	obb1Vertices[4] = obb1.center - obb1Length[0] + obb1Length[1] - obb1Length[2];
	//左下
	obb1Vertices[5] = obb1.center - obb1Length[0] - obb1Length[1] - obb1Length[2];
	//右上
	obb1Vertices[6] = obb1.center + obb1Length[0] + obb1Length[1] - obb1Length[2];
	//右下
	obb1Vertices[7] = obb1.center + obb1Length[0] - obb1Length[1] - obb1Length[2];

	//obb2の各頂点
	std::array<Vector3, 8> obb2Vertices;
	//obb1のセンターからの長さ
	std::array<Vector3, 3>obb2Length = {
		obb2.orientations[0] * obb2.size.x,
		obb2.orientations[1] * obb2.size.y,
		obb2.orientations[2] * obb2.size.z
	};
	//背面
	//左上
	obb2Vertices[0] = obb2.center - obb2Length[0] + obb2Length[1] + obb2Length[2];
	//左下
	obb2Vertices[1] = obb2.center - obb2Length[0] - obb2Length[1] + obb2Length[2];
	//右上
	obb2Vertices[2] = obb2.center + obb2Length[0] + obb2Length[1] + obb2Length[2];
	//右下
	obb2Vertices[3] = obb2.center + obb2Length[0] - obb2Length[1] + obb2Length[2];

	//正面
	//左上
	obb2Vertices[4] = obb2.center - obb2Length[0] + obb2Length[1] - obb2Length[2];
	//左下
	obb2Vertices[5] = obb2.center - obb2Length[0] - obb2Length[1] - obb2Length[2];
	//右上
	obb2Vertices[6] = obb2.center + obb2Length[0] + obb2Length[1] - obb2Length[2];
	//右下
	obb2Vertices[7] = obb2.center + obb2Length[0] - obb2Length[1] - obb2Length[2];

	for (const Vector3& separationAxis : separationAxes) {
		//ゼロ軸を無視
		Vector3 axis = separationAxis;
		if (axis.Length() < 1e-6f) {
			continue;
		}
		axis = axis.Normalize();

		//最大値と最小値の宣言
		float obb1Max = std::numeric_limits<float>::lowest();
		float obb1Min = std::numeric_limits<float>::max();
		float obb2Max = obb1Max;
		float obb2Min = obb1Min;

		//obb1を分離軸で射影したときの最大値と最小値を求める
		for (const Vector3& obb1Vertex : obb1Vertices) {
			//すべての頂点を分離軸で射影する
			float projected = obb1Vertex.Dot(axis);
			//射影した点の最大値と最小値を求める
			obb1Max = std::max(obb1Max, projected);
			obb1Min = std::min(obb1Min, projected);
		}

		//obb2を分離軸で射影したときの最大値と最小値を求める
		for (const Vector3& obb2Vertex : obb2Vertices) {
			//すべての頂点を分離軸で射影する
			float projected = obb2Vertex.Dot(axis);
			//射影した点の最大値と最小値を求める
			obb2Max = std::max(obb2Max, projected);
			obb2Min = std::min(obb2Min, projected);
		}

		//差分が形状を分離軸に射影した長さ=L
		float length = (obb1Max - obb1Min) + (obb2Max - obb2Min);

		float longSpan = std::max(obb1Max, obb2Max) - std::min(obb1Min, obb2Min);

		if (length < longSpan) {
			//分離軸が見つかったので衝突していない
			return false;
		}
	}

	return true;
}

//平面と球の衝突判定
bool collision::IsCollision(const primitiveData::Plane& plane, const primitiveData::Sphere& sphere) {
	//球の中心点と平面との距離
	float distance = std::abs(plane.normal.Dot(sphere.center) - plane.distance);

	//衝突判定
	return distance <= sphere.radius;
}

//平面とAABBの衝突判定
bool collision::IsCollision(const primitiveData::Plane& plane, const primitiveData::AABB& aabb) {
	//AABBの中心と半径を取得
	Vector3 center = (aabb.min + aabb.max) / 2.0f;
	Vector3 halfSize = (aabb.max - aabb.min) / 2.0f;

	//AABBの中心点と平面との距離
	float distance = plane.normal.Dot(center) - plane.distance;

	//半径を求める
	float radius = plane.normal.Abs().Dot(halfSize);

	return std::abs(distance) <= radius;
}

//AABBが平面の外側に完全に出ているか
bool IsOutsidePlaneAABB(const primitiveData::Plane& plane, const primitiveData::AABB& aabb) {
	//AABBの中心
	Vector3 center = (aabb.min + aabb.max) / 2.0f;

	//AABBの半径
	Vector3 halfSize = (aabb.max - aabb.min) / 2.0f;

	//平面からAABB中心までの符号付距離
	float signedDistance = plane.normal.Dot(center) - plane.distance;

	//AABBを平面法線方向に投影した半径
	float radius = plane.normal.Abs().Dot(halfSize);

	//法線が視錐台の内側を向いている場合
	return signedDistance < -radius;
}

//視錐台と衝突判定
bool collision::IsCollision(const primitiveData::Frustum& frustum, const primitiveData::AABB& aabb) {
	for (const primitiveData::Plane& plane : frustum.planes) {
		//平面の外にAABBがいた場合
		if (IsOutsidePlaneAABB(plane, aabb)) {
			return false;
		}
	}

	return true;
}

HitInfo collision::GetHitInfo(const OBB& obb1, const OBB& obb2) {
	//衝突情報
	HitInfo hitInfo{
		.isCollision = false,
		.normal = {},
		.depth = std::numeric_limits<float>::max()
	};

	//分離軸
	std::array<Vector3, 15> separationAxes;

	//面の法線
	//obb1
	separationAxes[0] = obb1.orientations[0];
	separationAxes[1] = obb1.orientations[1];
	separationAxes[2] = obb1.orientations[2];

	//obb2
	separationAxes[3] = obb2.orientations[0];
	separationAxes[4] = obb2.orientations[1];
	separationAxes[5] = obb2.orientations[2];

	//各辺のクロス積
	separationAxes[6] = obb1.orientations[0].Cross(obb2.orientations[0]);
	separationAxes[7] = obb1.orientations[0].Cross(obb2.orientations[1]);
	separationAxes[8] = obb1.orientations[0].Cross(obb2.orientations[2]);

	separationAxes[9] = obb1.orientations[1].Cross(obb2.orientations[0]);
	separationAxes[10] = obb1.orientations[1].Cross(obb2.orientations[1]);
	separationAxes[11] = obb1.orientations[1].Cross(obb2.orientations[2]);

	separationAxes[12] = obb1.orientations[2].Cross(obb2.orientations[0]);
	separationAxes[13] = obb1.orientations[2].Cross(obb2.orientations[1]);
	separationAxes[14] = obb1.orientations[2].Cross(obb2.orientations[2]);

	//obb1の各頂点
	std::array<Vector3, 8> obb1Vertices;
	//obb1のセンターからの長さ
	std::array<Vector3, 3>obb1Length = {
		obb1.orientations[0] * obb1.size.x,
		obb1.orientations[1] * obb1.size.y,
		obb1.orientations[2] * obb1.size.z
	};
	//背面
	//左上
	obb1Vertices[0] = obb1.center - obb1Length[0] + obb1Length[1] + obb1Length[2];
	//左下
	obb1Vertices[1] = obb1.center - obb1Length[0] - obb1Length[1] + obb1Length[2];
	//右上
	obb1Vertices[2] = obb1.center + obb1Length[0] + obb1Length[1] + obb1Length[2];
	//右下
	obb1Vertices[3] = obb1.center + obb1Length[0] - obb1Length[1] + obb1Length[2];

	//正面
	//左上
	obb1Vertices[4] = obb1.center - obb1Length[0] + obb1Length[1] - obb1Length[2];
	//左下
	obb1Vertices[5] = obb1.center - obb1Length[0] - obb1Length[1] - obb1Length[2];
	//右上
	obb1Vertices[6] = obb1.center + obb1Length[0] + obb1Length[1] - obb1Length[2];
	//右下
	obb1Vertices[7] = obb1.center + obb1Length[0] - obb1Length[1] - obb1Length[2];

	//obb2の各頂点
	std::array<Vector3, 8> obb2Vertices;
	//obb1のセンターからの長さ
	std::array<Vector3, 3>obb2Length = {
		obb2.orientations[0] * obb2.size.x,
		obb2.orientations[1] * obb2.size.y,
		obb2.orientations[2] * obb2.size.z
	};
	//背面
	//左上
	obb2Vertices[0] = obb2.center - obb2Length[0] + obb2Length[1] + obb2Length[2];
	//左下
	obb2Vertices[1] = obb2.center - obb2Length[0] - obb2Length[1] + obb2Length[2];
	//右上
	obb2Vertices[2] = obb2.center + obb2Length[0] + obb2Length[1] + obb2Length[2];
	//右下
	obb2Vertices[3] = obb2.center + obb2Length[0] - obb2Length[1] + obb2Length[2];

	//正面
	//左上
	obb2Vertices[4] = obb2.center - obb2Length[0] + obb2Length[1] - obb2Length[2];
	//左下
	obb2Vertices[5] = obb2.center - obb2Length[0] - obb2Length[1] - obb2Length[2];
	//右上
	obb2Vertices[6] = obb2.center + obb2Length[0] + obb2Length[1] - obb2Length[2];
	//右下
	obb2Vertices[7] = obb2.center + obb2Length[0] - obb2Length[1] - obb2Length[2];

	for (const Vector3& separationAxis : separationAxes) {
		//ゼロ軸を無視
		Vector3 axis = separationAxis;
		if (axis.Length() < 1e-6f) {
			continue;
		}
		axis = axis.Normalize();

		//最大値と最小値の宣言
		float obb1Max = std::numeric_limits<float>::lowest();
		float obb1Min = std::numeric_limits<float>::max();
		float obb2Max = obb1Max;
		float obb2Min = obb1Min;

		//obb1を分離軸で射影したときの最大値と最小値を求める
		for (const Vector3& obb1Vertex : obb1Vertices) {
			//すべての頂点を分離軸で射影する
			float projected = obb1Vertex.Dot(axis);
			//射影した点の最大値と最小値を求める
			obb1Max = std::max(obb1Max, projected);
			obb1Min = std::min(obb1Min, projected);
		}

		//obb2を分離軸で射影したときの最大値と最小値を求める
		for (const Vector3& obb2Vertex : obb2Vertices) {
			//すべての頂点を分離軸で射影する
			float projected = obb2Vertex.Dot(axis);
			//射影した点の最大値と最小値を求める
			obb2Max = std::max(obb2Max, projected);
			obb2Min = std::min(obb2Min, projected);
		}

		//差分が形状を分離軸に射影した長さ=L
		float length = (obb1Max - obb1Min) + (obb2Max - obb2Min);

		float longSpan = std::max(obb1Max, obb2Max) - std::min(obb1Min, obb2Min);

		//重なり量
		float overlap = std::min(obb1Max, obb2Max) - std::max(obb1Min, obb2Min);

		if (length < longSpan) {
			//分離軸が見つかったので衝突していない
			return hitInfo;
		}

		if (overlap < hitInfo.depth) {
			hitInfo.depth = overlap;
			hitInfo.normal = axis;
		}
	}

	hitInfo.isCollision = true;

	Vector3 cornerDelta = obb2.center - obb1.center;
	if (cornerDelta.Dot(hitInfo.normal) < 0.0f) {
		hitInfo.normal = -hitInfo.normal;
	}

	return hitInfo;
}
