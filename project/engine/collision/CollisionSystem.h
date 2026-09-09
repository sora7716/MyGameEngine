#pragma once
#include <vector>
#include <memory>

//前方宣言
class GameObject;
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
	/// AABBコライダーを集める
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクトの配列</param>
	void CollectAABBCollider(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// AABBの衝突判定を確認
	/// </summary>
	void CheckCollisonAABB();
private://メンバ変数
	//AABBコライダー
	std::vector<AABBCollider*>aabbColliders_;
};

