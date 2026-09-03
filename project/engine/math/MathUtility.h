#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector4.h"
#include "PrimitiveData.h"
#include <vector>
#include <cmath>
#include <array>
#include <numbers>
#include <algorithm>
#include "Quaternion.h"

/// <summary>
/// 数学的な計算
/// </summary>
namespace mathUtility {
	/// <summary>
	/// トランスフォームノーマル
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <param name="m">マトリックス</param>
	/// <returns></returns>
	Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

	/// <summary>
	/// 線形補間
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <param name="t">媒介変数</param>
	/// <returns>線形補間</returns>
	Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <param name="t">媒介変数</param>
	/// <returns> 球面線形補間</returns>
	Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

	/// <summary>
	/// CatmullRom補間
	/// </summary>
	/// <param name="p0">点0の座標</param>
	/// <param name="p1">点1の座標</param>
	/// <param name="p2">点2の座標</param>
	/// <param name="p3">点3の座標</param>
	/// <param name= "t">点 1を0.0f点2を1.0fとした割合指定</param>
	/// <returns>CatmullRom補間</returns>
	Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

	/// <summary>
	/// CatmullRomスプライン曲線上の座標を得る
	/// </summary>
	/// <param name="points">制御点の集合</param>
	/// <param name="t">スプラインの全区間の中での割合指定[0,1]</param>
	/// <returns>CatmullRomスプライン曲線上</returns>
	Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

	/// <summary>
	/// 三次元のベジエ曲線
	/// </summary>
	/// <param name="points">制御点</param>
	/// <param name="t">フレーム</param>
	/// <returns>ベジエ曲線</returns>
	Vector3 Bezier(const Vector3* points, float t);

	/// <summary>
	/// 三次元ベジエ曲線(球面線形補間ver)
	/// </summary>
	/// <param name="points">制御点</param>
	/// <param name="t">フレーム</param>
	/// <returns>ベジエ曲線</returns>
	Vector3 BezierS(const Vector3* points, float t);

	/// <summary>
	/// リサージュ曲線
	/// </summary>
	/// <param name="theta">θ</param>
	/// <param name="center">中心点</param>
	/// <param name="scalar">スカラー</param>
	/// <returns>閉曲線</returns>
	Vector3 LissajousCurve(const Vector3& theta, const Vector3& center, const Vector3& scalar = { 1.0f,1.0f,1.0f });

	/// <summary>
	/// 逆正接関数のcotangent
	/// </summary>
	/// <param name="theta">θ</param>
	/// <returns>逆正接関数</returns>
	float Cont(float theta);

	/// <summary>
	/// 円運動(x,z)
	/// </summary>
	/// <param name="center">中心</param>
	/// <param name="radius">円運動の半径</param>
	/// <param name="theta">角度</param>
	Vector3 CircularMoveXZ(const Vector3& center, const Vector2& radius, float theta);

	/// <summary>
	/// 平面を作成(無限平面)
	/// </summary>
	/// <param name="p0">平面上の点0</param>
	/// <param name="p1">平面上の点1</param>
	/// <param name="p2">平面上の点2</param>
	/// <returns>平面</returns>
	primitiveData::Plane MakePlane(const Vector3& p0, const Vector3& p1, const Vector3& p2);

	/// <summary>
	/// 視錐台の頂点の作成
	/// </summary>
	/// <param name="nearClip">ニアクリップ距離</param>
	/// <param name="farClip">ファークリップ距離</param>
	/// <param name="fovY">fovY</param>
	/// <param name="aspect">アスペクト比</param>
	/// <returns>視錐台の頂点</returns>
	std::array<Vector3, 8>CreateFrustumVertex(float nearClip, float farClip, float fovY, float aspect);

	/// <summary>
	/// 視錐台の作成
	/// </summary>
	/// <param name="vertices">頂点</param>
	/// <param name="worldMatrix">ワールド行列</param>
	/// <returns>視錐台</returns>
	primitiveData::Frustum CreateFrustumData(const std::array<Vector3, 8>& vertices, const Matrix4x4& worldMatrix);

	/// <summary>
	/// 平行四辺形の面積を求める
	/// </summary>
	/// <param name="vertices">頂点</param>
	/// <returns>平行四辺形の面積</returns>
	float CalcParallelogramArea(const std::array<Vector3, 3>& vertices);

	/// <summary>
	/// 三角形の面積を求める
	/// </summary>
	/// <param name="vertices">頂点</param>
	/// <returns>三角形の面積</returns>
	float CalcTriangleArea(const std::array<Vector3, 3>& vertices);

	/// <summary>
	/// 平行四辺形の面積を処理を早くして(正確じゃない)
	/// </summary>
	/// <param name="vertices">頂点</param>
	/// <returns>三角形の面積(正確じゃない)</returns>
	float CalcParallelogramAreaSquared(const std::array<Vector3, 3>& vertices);

	/// <summary>
	/// クォータニオンからオイラー角を求める
	/// </summary>
	/// <param name="quaternion">クォータニオン</param>
	/// <returns>オイラー角</returns>
	Vector3 MakeEulerAngleForQuaternion(const Quaternion& quaternion);

	//定数
	//デルタタイム
	inline constexpr float kDeltaTime = 1.0f / 60.0f;
	//円周率
	inline constexpr float kPi = std::numbers::pi_v<float>;
	//ラジアン変換用定数
	inline constexpr float kRad = std::numbers::pi_v<float> / 180.0f;
};
