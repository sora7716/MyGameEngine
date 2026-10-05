#define NOMINMAX
#include "CollisionSystem.h"
#include "GameObject.h"
#include "AABBCollider.h"
#include "Collision.h"
#include "RigidBody.h"

//コンストラクタ
CollisionSystem::CollisionSystem(ConstructorKey){
}

//デストラクタ
CollisionSystem::~CollisionSystem(){
}

//更新
void CollisionSystem::Update(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//コライダーを集める
	CollectCollider(gameObjects);

	//衝突判定をする前準備
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		RigidBody* rigidBody = gameObject->GetComponent<RigidBody>();

		if (rigidBody){
			rigidBody->BeginCollisionFrame();
		}
	}

	//過去のペアに保存
	previousCollisions_ = currentCollisions_;
	//現在のペアをクリア
	currentCollisions_.clear();

	//コライダーの数分衝突判定を確認
	for (uint32_t i = 0; i < static_cast<uint32_t>(colliders_.size()); i++){
		BaseCollider* collider1 = colliders_[i];

		for (uint32_t j = i + 1; j < static_cast<uint32_t>(colliders_.size()); j++){
			BaseCollider* collider2 = colliders_[j];

			//同じGameObjectをにくっついていた場合
			if (collider1->GetOwner() == collider2->GetOwner()){
				continue;
			}

			//衝突を確認
			CheckCollisionPair(collider1, collider2);
		}
	}

	////前フレームから離れたペアを探す
	for (const CollisionRecord& previous : previousCollisions_){
		//探す
		auto found = std::find(currentCollisions_.begin(), currentCollisions_.end(), previous);

		//現在も存在するなら
		if (found != currentCollisions_.end()){
			continue;
		}

		//isTriggerがtrueなら
		if (previous.isTrigger){
			previous.collider1->GetOwner()->InvokeTriggerEvent(static_cast<uint32_t>(CollisionState::kExit), previous.collider2);
			previous.collider2->GetOwner()->InvokeTriggerEvent(static_cast<uint32_t>(CollisionState::kExit), previous.collider1);
		} else{
			previous.collider1->GetOwner()->InvokeCollisionEvent(static_cast<uint32_t>(CollisionState::kExit), previous.info1);
			previous.collider2->GetOwner()->InvokeCollisionEvent(static_cast<uint32_t>(CollisionState::kExit), previous.info2);
		}
	}
}

//参照を解除
void CollisionSystem::RemoveReflectionTo(GameObject* target){
	//コライダーが存在しているか
	BaseCollider* targetCollider = target->GetComponent<BaseCollider>();
	if (!targetCollider){
		return;
	}

	//ゲームオブジェクトから削除
	colliders_.erase(
		std::remove_if(
			colliders_.begin(),
			colliders_.end(),
			[target](BaseCollider* collider){
				return collider->GetOwner() == target;
			}
		),
		colliders_.end()
	);
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
	if (type1 == ColliderType::kBox && type2 == ColliderType::kBox){
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
	Vector3 overlap = {};
	overlap.x = std::min(aabb1.max.x, aabb2.max.x) - std::max(aabb1.min.x, aabb2.min.x);
	overlap.y = std::min(aabb1.max.y, aabb2.max.y) - std::max(aabb1.min.y, aabb2.min.y);
	overlap.z = std::min(aabb1.max.z, aabb2.max.z) - std::max(aabb1.min.z, aabb2.min.z);

	//めり込み度を代入
	float depth = std::min({ overlap.x,overlap.y,overlap.z });
	//差分を見てnormalの方向を決める
	collisionInfo1.normal = CalculatePushOutNormal(translate1 - translate2, overlap);
	collisionInfo2.normal = CalculatePushOutNormal(translate2 - translate1, overlap);

	//めり込み度を代入
	collisionInfo1.penetrationDepth = depth;
	collisionInfo2.penetrationDepth = depth;

	//衝突対象を設定
	collisionInfo1.other = collider2;
	collisionInfo2.other = collider1;

	//コライダーのペアを登録
	CollisionRecord collisionRecord = RegisterColliderPair(collider1, collider2, collisionInfo1, collisionInfo2);

	//衝突状況の判断
	collisionState_ = JudgeCollisionState(collisionRecord);

	//ゲームオブジェクトを取得
	GameObject* gameObject1 = collider1->GetOwner();
	GameObject* gameObject2 = collider2->GetOwner();

	//衝突していた場合
	if (collider1->IsTrigger() || collider2->IsTrigger()){
		//どちらか片方がIsTriggerがtrueだった場合
		gameObject1->InvokeTriggerEvent(static_cast<uint32_t>(collisionState_), collider2);
		gameObject2->InvokeTriggerEvent(static_cast<uint32_t>(collisionState_), collider1);
	} else{
		//オブジェクトを押し出す
		ResolveCollision(collider1, collider2, collisionInfo1, collisionInfo2);

		//どちらか片方がIsTriggerがfalseだった場合
		gameObject1->InvokeCollisionEvent(static_cast<uint32_t>(collisionState_), collisionInfo1);
		gameObject2->InvokeCollisionEvent(static_cast<uint32_t>(collisionState_), collisionInfo2);
	}
}

//押し出す方向を取得
Vector3 CollisionSystem::CalculatePushOutNormal(const Vector3& diff, const Vector3& overlap){
	//反発する方向を代入
	Vector3 normal = {};
	//どの軸を使用するか決める
	if (overlap.x <= overlap.y && overlap.x <= overlap.z){
		normal.x = 1.0f;
		if (diff.x < 0.0f){
			normal.x *= -1.0f;
		}
	} else if (overlap.y <= overlap.z){
		normal.y = 1.0f;
		if (diff.y < 0.0f){
			normal.y *= -1.0f;
		}
	} else{
		normal.z = 1.0f;
		if (diff.z < 0.0f){
			normal.z *= -1.0f;
		}
	}

	return normal;
}

//オブジェクトの押し出し
void CollisionSystem::ResolveCollision(BaseCollider* collider1, BaseCollider* collider2, const CollisionInfo& info1, const CollisionInfo& info2){
	//ボディタイプ
	BodyType bodyType1 = collider1->GetBodyType();
	BodyType bodyType2 = collider2->GetBodyType();

	//1の移動係数
	float moveFactor1 = 0.0f;
	if (bodyType1 == BodyType::kDynamic){
		moveFactor1 = 1.0f;
	}

	//2の移動係数
	float moveFactor2 = 0.0f;
	if (bodyType2 == BodyType::kDynamic){
		moveFactor2 = 1.0f;
	}

	//移動係数の合計
	float moveFactorSum = moveFactor1 + moveFactor2;

	//static同士だった場合
	if (moveFactorSum <= 0.0f){
		return;
	}

	//Transformを取得
	Transform& transform1 = collider1->GetOwner()->GetTransform();
	Transform& transform2 = collider2->GetOwner()->GetTransform();

	//移動係数の割合を取得
	float rate1 = moveFactor1 / moveFactorSum;
	float rate2 = moveFactor2 / moveFactorSum;

	//押し戻しをする
	transform1.translate += info1.normal * info1.penetrationDepth * rate1;
	transform2.translate += info2.normal * info2.penetrationDepth * rate2;
}

//コライダーのペアを登録
CollisionRecord CollisionSystem::RegisterColliderPair(BaseCollider* collider1, BaseCollider* collider2, const CollisionInfo& info1, const CollisionInfo& info2){
	//ペアを保存
	std::less<BaseCollider*>compere;
	//衝突判定情報のレコード
	CollisionRecord collisionRecord = {};
	if (compere(collider1, collider2)){
		collisionRecord.collider1 = collider1;
		collisionRecord.collider2 = collider2;
		collisionRecord.info1 = info1;
		collisionRecord.info2 = info2;
	} else{
		collisionRecord.collider1 = collider2;
		collisionRecord.collider2 = collider1;
		collisionRecord.info1 = info2;
		collisionRecord.info2 = info1;
	}

	//Triggerがtrueかfalseかを見る
	collisionRecord.isTrigger = collider1->IsTrigger() || collider2->IsTrigger();

	//ペアの保存
	currentCollisions_.push_back(collisionRecord);

	return collisionRecord;
}

//コライダーのペアを見て衝突状況を判断
CollisionState CollisionSystem::JudgeCollisionState(const CollisionRecord& collisionRecord){
	//現在のペアに存在しているか
	bool isCurrentPair = std::find(currentCollisions_.begin(), currentCollisions_.end(), collisionRecord) != currentCollisions_.end();

	//過去のペアに存在しているか
	bool isPreviousPair = std::find(previousCollisions_.begin(), previousCollisions_.end(), collisionRecord) != previousCollisions_.end();

	//衝突状況を判断
	if (isCurrentPair && isPreviousPair){
		return CollisionState::kStay;
	} else if (isCurrentPair && !isPreviousPair){
		return CollisionState::kEnter;
	}
	return CollisionState::kExit;
}

//一致した場合
bool CollisionRecord::operator==(const CollisionRecord& other)const{
	return collider1 == other.collider1 && collider2 == other.collider2;
}
