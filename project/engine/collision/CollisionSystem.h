#pragma once
#include <vector>
#include <memory>
#include "Vector3.h"
#include "BaseCollider.h"

//前方宣言
class GameObject;
class BoxCollider;

/// <summary>
/// 衝突状況
/// </summary>
enum class CollisionState :uint32_t{
	kEnter,
	kStay,
	kExit,
	kCount
};

//衝突の情報を記録しておくため
struct CollisionRecord{
	//コライダー
	BaseCollider* collider1 = nullptr;
	BaseCollider* collider2 = nullptr;
	//衝突情報
	CollisionInfo info1 = {};
	CollisionInfo info2 = {};
	//押し戻すか
	bool isTrigger = false;

	//一致した場合
	bool operator==(const CollisionRecord& other)const;
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

	/// <summary>
	/// 参照を解除
	/// </summary>
	/// <param name="target">対象</param>
	void RemoveReflectionTo(GameObject* target);
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
	/// Boxの衝突判定を確認
	/// </summary>
	/// <param name="collider1">コライダー1</param>
	/// <param name="collider2">コライダー2</param>
	void CheckCollisionAABB(BoxCollider* collider1, BoxCollider* collider2);

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
	/// <param name="info1">衝突したときの情報1</param>
	/// <param name="info2">衝突したときの情報2</param>
	/// <returns>衝突判定の記録</returns>
	CollisionRecord RegisterColliderPair(BaseCollider* collider1, BaseCollider* collider2, const CollisionInfo& info1, const CollisionInfo& info2);

	/// <summary>
	/// 衝突判定の記録を見て衝突状況を判断
	/// </summary>
	/// <param name="collisionRecord">衝突判定の記録</param>
	/// <returns>衝突状況</returns>
	CollisionState JudgeCollisionState(const CollisionRecord& collisionRecord);
private://メンバ変数
	//コライダー
	std::vector<BaseCollider*>colliders_;
	//現在のコライダー
	std::vector<CollisionRecord>currentCollisions_;
	//前フレームのコライダー
	std::vector<CollisionRecord>previousCollisions_;
	//衝突状況
	CollisionState collisionState_ = CollisionState::kCount;
};

