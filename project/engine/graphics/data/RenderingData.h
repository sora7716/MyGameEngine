#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include <string>
#include <vector>
//Transform情報
struct Transform{
	Vector3 scale = Vector3::GetOne();
	Quaternion rotate = Quaternion::IdentityQuaternion();
	Vector3 translate = {};

	/// <summary>
	/// オイラー角の設定
	/// </summary>
	/// <param name="eulerAngle">オイラー角</param>
	void SetEulerAngle(const Vector3& eulerAngle);

	/// <summary>
	/// オイラー角(度数法)の設定
	/// </summary>
	/// <param name="degreeAngle">度数法</param>
	void SetEulerAngleDegrees(const Vector3& degreeAngle);

	/// <summary>
	/// クォータニオンの設定
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	void SetRotate(const Quaternion& quaternion);

	/// <summary>
	/// オイラー角の取得
	/// </summary>
	/// <returns>オイラー角</returns>
	Vector3 GetEulerAngle();

	/// <summary>
	/// 度数法での角度の取得
	/// </summary>
	/// <returns>度数法での角度</returns>
	Vector3 GetDegreeAngle();

	/// <summary>
	/// 補間
	/// </summary>
	/// <param name="transform1">トランスフォーム1</param>
	/// <param name="transform2">トランスフォーム2</param>
	/// <param name="t">係数</param>
	/// <returns>補間後のトランスフォーム</returns>
	static Transform Lerp(const Transform& transform1, const Transform& transform2, float t);
};

//Transform2D情報
struct RectTransform{
	Vector2 scale = Vector2::GetOne();
	float rotate = 0.0f;
	Vector2 translate = {};
};

//TransformationMatrix
struct TransformationMatrix{
	Matrix4x4 world = Matrix4x4::Identity4x4();
	Matrix4x4 worldInverseTranspose = Matrix4x4::Identity4x4();
};

//TransformationMatrix
struct TransformationMatrixForSprite{
	Matrix4x4 wvp = Matrix4x4::Identity4x4();
	Matrix4x4 world = Matrix4x4::Identity4x4();
};

//ノード構造体
struct Node{
	Matrix4x4 baseMatrix = Matrix4x4::Identity4x4();//読み込み時の元の姿勢
	Matrix4x4 localMatrix = Matrix4x4::Identity4x4();//こちらで操作する用の行列
	Matrix4x4 modelMatrix = Matrix4x4::Identity4x4();//他のNodeも合わせたモデル全体の姿勢
	Transform localTransform = {};
	std::string name = "";
	std::vector<Node> children;
	std::vector<uint32_t> meshIndices;
};