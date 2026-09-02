#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include <string>
#include <vector>
//Transform情報
struct Transform{
	Vector3 scale = Vector3::MakeAllOne();
	Quaternion quaternion = Quaternion::IdentityQuaternion();
	Vector3 translate = {};
	Vector3 eulerAngle = {};

	/// <summary>
	/// オイラー角の設定
	/// </summary>
	/// <param name="rotate">オイラー角</param>
	void SetEulerAngle(const Vector3& rotate);
};

//Transform2D情報
struct RectTransform{
	Vector2 scale = Vector2::MakeAllOne();
	float rotate = 0.0f;
	Vector2 translate = {};
};

//TransformationMatrix
struct TransformationMatrix{
	Matrix4x4 wvp = {};
	Matrix4x4 world = {};
	Matrix4x4 worldInverseTranspose = {};
};

//TransformationMatrix
struct TransformationMatrixForSprite{
	Matrix4x4 wvp = {};
	Matrix4x4 world = {};
};


//ノード構造体
struct Node{
	Matrix4x4 localMatrix = {};
	std::string name = "";
	std::vector<Node> children;
};