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

//AABBのコライダーを集める
void CollisionSystem::CollectAABBCollider(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//描画に有効なAABBのコライダーをリセット
	aabbColliders_.clear();

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

		//AABBのコライダーの配列を取得
		std::vector<AABBCollider*> aabbColliders = gameObject->GetComponents<AABBCollider>();

		//AABBコライダーをメンバ変数に追加
		for (AABBCollider* aabbCollider : aabbColliders){
			//有効じゃなければ
			if (!aabbCollider->IsEnabled()){
				continue;
			}

			//メンバ変数に追加
			aabbColliders.push_back(aabbCollider);
		}
	}
}

//AABBの衝突判定を確認
void CollisionSystem::CheckCollisonAABB(){
	for (uint32_t i = 0; i < static_cast<uint32_t>(aabbColliders_.size()); i++){
		//AABBコライダー1
		AABBCollider* aabbCollider1 = aabbColliders_[i];
		for (uint32_t j = i + 1; j < static_cast<uint32_t>(aabbColliders_.size()); j++){
			//AABBコライダー2
			AABBCollider* aabbCollider2 = aabbColliders_[j];

			//同じゲームオブジェクトについていた場合排除
			if (aabbCollider1->GetOwner() == aabbCollider2->GetOwner()){
				continue;
			}

			//衝突したかどうか
			bool isCollision = collision::IsCollision(aabbCollider1->GetAABB(), aabbCollider2->GetAABB());

			//衝突していなかった場合
			if (!isCollision){
				continue;
			}
		}
	}
}
