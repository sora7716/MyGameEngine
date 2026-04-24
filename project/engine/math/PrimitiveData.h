#pragma once
#include "Vector3.h"
#include "Quaternion.h"

//球のデータ
struct Sphere {
	Vector3 center; //中心座標
	float radius;   //半径
};

//線分
struct Segment {
	Vector3 origin;
	Vector3 diff;
};

//AABB
struct AABB {
	Vector3 min;//最小値
	Vector3 max;//最大値
};

//OBB
struct OBB {
	Vector3 center;//中心点
	Quaternion quaternion;//回転
	Vector3 orientations[3];//座法軸。正規化・直行必須
	Vector3 size;//座標軸方向の長さ半分。中心から面までの距離
};
