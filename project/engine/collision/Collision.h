#pragma once
#include "PrimitiveData.h"

/// <summary>
/// 衝突判定
/// </summary>
namespace collision {
	/// <summary>
	/// 球同士の衝突判定
	/// </summary>
	/// <param name="sphere1">球1</param>
	/// <param name="sphere2">球2</param>
	/// <returns>衝突したかどうか</returns>
	bool IsCollision(const primitiveData::Sphere& sphere1, const primitiveData::Sphere& sphere2);

	/// <summary>
	/// AABB同士の衝突判定
	/// </summary>
	/// <param name="aabb1">aabb1</param>
	/// <param name="aabb2">aabb2</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::AABB& aabb1, const primitiveData::AABB& aabb2);

	/// <summary>
	/// AABBと球の衝突判定
	/// </summary>
	/// <param name="aabb">aabb</param>
	/// <param name="sphere">球</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::AABB& aabb, const primitiveData::Sphere& sphere);

	/// <summary>
	/// OBBと球の衝突判定
	/// </summary>
	/// <param name="obb">obb</param>
	/// <param name="sphere">球</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::OBB& obb, const primitiveData::Sphere& sphere);

	/// <summary>
	/// 分離軸を使用したOBB同士の衝突判定
	/// </summary>
	/// <param name="obb1">obb1</param>
	/// <param name="obb2">obb2</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::OBB& obb1, const primitiveData::OBB& obb2);

	/// <summary>
	/// 平面と球の衝突判定
	/// </summary>
	/// <param name="plane">平面</param>
	/// <param name="sphere">球</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::Plane& plane, const primitiveData::Sphere& sphere);

	/// <summary>
	/// 平面とAABBの衝突判定
	/// </summary>
	/// <param name="plane">平面</param>
	/// <param name="aabb">AABB</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::Plane& plane, const primitiveData::AABB& aabb);

	/// <summary>
	/// 視錐台とAABBの衝突判定
	/// </summary>
	/// <param name="frustum">視錐台</param>
	/// <param name="aabb">AABB</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const primitiveData::Frustum& frustum, const primitiveData::AABB& aabb);
};