#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include <array>
#include "Matrix4x4.h"

namespace PrimitiveData {
	//球のデータ
	struct Sphere {
		Vector3 center = {}; //中心座標
		float radius = 0.0f;//半径
	};

	//円のデータ
	struct Circle {
		Vector3 center = {};//中心座標
		Vector3 eulerAngle = {};//オイラー角
		float radius = 0.0f;//半径
	};

	//線分
	struct Segment {
		Vector3 origin = {};//始点
		Vector3 diff = {};//始点から終点までの差分
	};

	//AABB
	struct AABB {
		Vector3 min = {};//最小値
		Vector3 max = {};//最大値

		/// <summary>
		/// 行列との掛け算
		/// </summary>
		/// <param name="m">行列</param>
		/// <returns>AABB</returns>
		AABB operator*(const Matrix4x4& m)const;
	};

	//OBB
	struct OBB {
		Vector3 center = {};//中心点
		Quaternion quaternion = Quaternion::IdentityQuaternion();//回転
		Vector3 orientations[3] = {
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f}
		};//座法軸。正規化・直行必須
		Vector3 size = {};//座標軸方向の長さ半分。中心から面までの距離

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
	};

	//Plane
	struct Plane {
		Vector3 normal = {};//法線
		float distance = 0.0f;//距離
	};

	//視錐台
	struct Frustum {
		enum PlaneIndex {
			kLeft,
			kRight,
			kTop,
			kBottom,
			kNear,
			kFar,
			kCount
		};
		std::array<Plane, kCount>planes = {};//面
		std::array<Vector3, 8>localCorners = {};//ローカルの頂点
		std::array<Vector3, 8>worldCorners = {};//ワールドの頂点
	};
}
