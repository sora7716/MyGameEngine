#pragma once
#include "ColliderData.h"
#include "algorithms/Collision.h"
#include <vector>
#include <cstdint>

/// <summary>
/// 衝突の管理
/// </summary>
class ColliderManager {
public://メンバ関数
	/// <summary>
	/// コライダーを追加
	/// </summary>
	/// <param name="collider">追加するコライダー</param>
	void AddCollider(Collider* collider);

	/// <summary>
	/// コライダーを削除
	/// </summary>
	/// <param name="collider">削除するコライダー</param>
	void RemoveCollider(Collider* collider);

	/// <summary>
	/// 衝突判定を行う
	/// </summary>
	void ProcessCollision();

	/// <summary>
	/// Bitに変換
	/// </summary>
	/// <param name="layer">レイヤー</param>
	/// <returns>レイヤーのBit</returns>
	static uint32_t ToBit(Layer layer);
private://メンバ関数
	/// <summary>
	/// 追加していいか
	/// </summary>
	/// <param name="collider">コライダー</param>
	/// <returns>追加していいか</returns>
	bool IsRegistered(Collider* collider);

	/// <summary>
	/// コライダーをOBBに同期
	/// </summary>
	void SyncCollider();

	/// <summary>
	/// 衝突判定をチェック
	/// </summary>
	void CheckCollision();

	/// <summary>
    /// 衝突判定を行えるか
    /// </summary>
	/// <param name="collider">コライダー</param>
    /// <returns>衝突判定を行えるか</returns>
	bool IsActive(Collider* collider);

	/// <summary>
	/// 押し出し
	/// </summary>
	/// <param name="self">対象</param>
	/// <param name="other">それ以外</param>
	/// <param name="hit">衝突したかの情報</param>
	void Resolve(ColliderState& self, const ColliderState& other, HitInfo hit);

	/// <summary>
    /// Layerを使った衝突判定
    /// </summary>
    /// <param name="self">対象</param>
    /// <param name="other">それ以外</param>
	/// <returns>Layerを使った衝突判定</returns>
	bool IsLayerCollidable(const Collider* self, const Collider* other);
private://メンバ変数
	std::vector<Collider*>colliders_;
};

