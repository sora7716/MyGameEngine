#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include <string>
#include <vector>
//Transform情報
struct TransformData {
	Vector3 scale;
	Quaternion quaternion;
	Vector3 translate;

	//デバック用
	//軸
	Vector3 axis;
	//角度
	float angle;
	//オイラー角
	Vector3 eulerAngle;
	//クォータニオンかオイラーか
	bool isUsingQuaternion;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
};

//Transform2D情報
struct Transform2dData {
	Vector2 scale;
	float rotate;
	Vector2 translate;
	
	/// <summary>
    /// 初期化
    /// </summary>
	void Initialize();
};

//TransformationMatrix
struct TransformationMatrix {
	Matrix4x4 wvp;
	Matrix4x4 world;
	Matrix4x4 worldInverseTranspose;
};

//ノード構造体
struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};