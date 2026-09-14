#pragma once
#include <vector>
#include <memory>
#include "Vector3.h"

//前方宣言
class GameObject;
class BaseCollider;
class AABBCollider;
struct CollisionInfo;

/// <summary>
/// 衝突状況
/// </summary>
enum class CollisionState :uint32_t{
	kEnter,
	kStay,
	kExit,
	kCount
};

/// <summary>
/// 衝突判定のシステム
/// </summary>
class CollisionSystem{
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit CollisionSystem(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~CollisionSystem();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::vector<std::unique_ptr<GameObject>>& gameObjects);
private://メンバ関数
	//コピーコンストラクタ禁止
	CollisionSystem(const CollisionSystem&) = delete;
	//代入演算子の禁止
	CollisionSystem& operator=(const CollisionSystem&) = delete;

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

	/// <summary>
	/// 押し出す方向を取得
	/// </summary>
	/// <param name="diff">差分</param>
	/// <param name="overlap">どれくらい重なってるか</param>
	/// <returns>押し出す方向</returns>
	Vector3 CalculatePushOutNormal(const Vector3& diff, const Vector3& overlap);

	/// <summary>
	/// オブジェクトの押し出し
	/// </summary>
	/// <param name="collider1">コライダー1</param>
	/// <param name="collider2">コライダー2</param>
	/// <param name="info1">衝突したときの情報1</param>
	/// <param name="info2">衝突したときの情報2</param>
	void ResolveCollision(BaseCollider* collider1, BaseCollider* collider2, const CollisionInfo& info1, const CollisionInfo& info2);

	/// <summary>
	/// コライダーのペアを登録
	/// </summary>
	/// <param name="collider1">コライダー1</param>
	/// <param name="collider2">コライダー2</param>
	/// <returns>ペア</returns>
	std::pair<BaseCollider*, BaseCollider*> RegisterColliderPair(BaseCollider* collider1, BaseCollider* collider2);

	/// <summary>
	/// コライダーのペアを見て衝突状況を判断
	/// </summary>
	/// <param name="pair">コライダーのペア</param>
	/// <returns>衝突状況</returns>
	CollisionState JudgeCollisionState(const std::pair<BaseCollider*, BaseCollider*>& pair);
private://メンバ変数
	//コライダー
	std::vector<BaseCollider*>colliders_;
	//コライダーのペア
	std::vector<std::pair<BaseCollider*, BaseCollider*>>currentColliderPairs_;
	//前フレームのペア
	std::vector<std::pair<BaseCollider*, BaseCollider*>>previousColliderPairs_;
	//衝突状況
	CollisionState collisionState_ = CollisionState::kCount;
};

