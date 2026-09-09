#pragma once
#include <vector>
#include <memory>

//前方宣言
class GameObject;
class BaseCollider;
class AABBCollider;

/// <summary>
/// 衝突判定のシステム
/// </summary>
class CollisionSystem{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CollisionSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CollisionSystem();
private://メンバ関数
	/// <summary>
	/// コライダーを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectCollider(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// コライダーのペアを確認
	/// </summary>
	/// <param name="collider1">コライダー1</param>
	/// <param name="collider2">コライダー2</param>
	void CheckCollisionPair(BaseCollider* collider1, BaseCollider* collider2);

	/// <summary>
	/// AABBの衝突判定を確認
	/// </summary>
	/// <param name="collider1">コライダー1</param>
	/// <param name="collider2">コライダー2</param>
	void CheckCollisionAABB(AABBCollider* collider1, AABBCollider* collider2);
private://メンバ変数
	//コライダー
	std::vector<BaseCollider*>colliders_;
};

