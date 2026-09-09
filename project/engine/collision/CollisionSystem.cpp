#define NOMINMAX
#include "CollisionSystem.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "Collision.h"

//コンストラクタ
CollisionSystem::CollisionSystem(){
}

//デストラクタ
CollisionSystem::~CollisionSystem(){
}

//コライダーを集める
void CollisionSystem::CollectCollider(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//描画に有効なコライダーをリセット
	colliders_.clear();

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//コライダーの配列を取得
		std::vector<BaseCollider*> colliders = gameObject->GetComponents<BaseCollider>();

		//コライダーをメンバ変数に追加
		for (BaseCollider* collider : colliders){
			//有効じゃなければ
			if (!collider->IsEnabled()){
				continue;
			}

			//メンバ変数に追加
			colliders_.push_back(collider);
		}
	}
}

//コライダーのペアを確認
void CollisionSystem::CheckCollisionPair(BaseCollider* collider1, BaseCollider* collider2){
	//コライダーのタイプを取得
	ColliderType type1 = collider1->GetColliderType();
	ColliderType type2 = collider2->GetColliderType();

	//両方AABBかどうかを確認
	if (type1 == ColliderType::kAABB && type2 == ColliderType::kAABB){
		//AABBの衝突判定
		CheckCollisionAABB(static_cast<AABBCollider*>(collider1), static_cast<AABBCollider*>(collider2));
	}
}

//AABBの衝突判定を確認
void CollisionSystem::CheckCollisionAABB(AABBCollider* collider1, AABBCollider* collider2){
	//AABBを取得
	primitiveData::AABB aabb1 = collider1->GetAABB();
	primitiveData::AABB aabb2 = collider2->GetAABB();

	//衝突判定
	bool isCollision = collision::IsCollision(aabb1, aabb2);

	//衝突していなかったら
	if (!isCollision){
		return;
	}

	//衝突したときの情報
	CollisionInfo collisionInfo1 = {};
	CollisionInfo collisionInfo2 = {};

	//位置ベクトルを取得
	const Vector3& translate1 = collider1->GetOwner()->GetTransform().translate;
	const Vector3& translate2 = collider2->GetOwner()->GetTransform().translate;

	//めり込みを確認
	float overlapX = std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x);
	float overlapY = std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y);
	float overlapZ = std::min(aabb1.max.z, aabb2.max.z) - std::max(aabb1.min.z, aabb2.min.z);

	//めり込み度を代入
	float depth = std::min({ overlapX,overlapY,overlapZ });
	//反発する方向を代入
	collisionInfo1.normal = (translate2 - translate1).Normalize();

	//衝突していた場合
	if (collider1->IsTrigger() || collider2->IsTrigger()){
		//どちらか片方がIsTriggerがtrueだった場合
		collider1->OnTrigger(collider2);
		collider2->OnTrigger(collider1);
	} else{
		//どちらか片方がIsTriggerがfalseだった場合
		collider1->OnCollision();
		collider2->OnCollision();
	}
}
