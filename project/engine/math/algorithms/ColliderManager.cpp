#include "ColliderManager.h"
#include "algorithms/Collision.h"
#include "WireframeObject3d.h"
#include "Object3d.h"

//コライダーの追加
void ColliderManager::AddCollider(Collider* collider) {
	//追加されているか確認
	if (!IsRegistered(collider)) {
		return;
	}

	//追加されていなかったら追加
	colliders_.push_back(collider);
}

//コライダーの削除
void ColliderManager::RemoveCollider(Collider* collider) {
	std::vector<Collider*>::iterator itCollider = std::remove(colliders_.begin(), colliders_.end(), collider);
	colliders_.erase(itCollider, colliders_.end());
}

//衝突判定を行う
void ColliderManager::ProcessCollision() {
	//コライダーをOBBに同期
	SyncCollider();
	//衝突判定をチェック
	CheckCollision();
}

//Bitに変換
uint32_t ColliderManager::ToBit(Layer layer) {
	return static_cast<uint32_t>(layer);
}

//追加していいか
bool ColliderManager::IsRegistered(Collider* collider) {
	//colliderがnullだったら追加しない
	if (!collider) {
		return false;
	}
	//すでに登録されているかを確認
	std::vector<Collider*>::iterator itCollider = std::find(colliders_.begin(), colliders_.end(), collider);
	if (itCollider != colliders_.end()) {
		return false;
	}

	return true;
}

//コライダーをOBBに同期
void ColliderManager::SyncCollider() {
	for (Collider* collider : colliders_) {
		if (!collider) {
			continue;
		} else if (!collider->isEnabled) {
			continue;
		} else if (!collider->owner) {
			continue;
		}

		//OBBの値の更新
		collider->obb.size = *collider->owner->scalePtr;
		collider->obb.quaternion = *collider->owner->rotatePtr;
		Rendering::MakeOBBRotateMatrix(collider->obb.orientations, collider->obb.quaternion);
		collider->obb.center = *collider->owner->translatePtr;
	}
}

//衝突判定をチェック
void ColliderManager::CheckCollision() {
	for (uint32_t i = 0; i < colliders_.size(); i++) {
		Collider* pairA = colliders_[i];
		if (!IsActive(pairA)) {
			continue;
		}

		for (uint32_t j = i + 1; j < colliders_.size(); j++) {
			Collider* pairB = colliders_[j];
			if (!IsActive(pairB)) {
				continue;
			}
			//衝突情報
			HitInfo hitInfo = Collision::GetHitInfo(pairA->obb, pairB->obb);
			if (!hitInfo.isCollision) {
				continue;
			}

			//衝突した時に行う判定
			if (pairA->onCollision) {
				pairA->onCollision(pairB->owner);
			}

			if (pairB->onCollision) {
				pairB->onCollision(pairA->owner);
			}

			//Layer/Mask判定
			if (!IsLayerCollidable(pairA, pairB)) {
				continue;
			}

			//Triggerが絡むなら押し出しをしない
			if (pairA->isTrigger || pairB->isTrigger) {
				continue;
			}

			//Static同士何もしない
			if (pairA->bodyType == BodyType::kStatic && pairB->bodyType == BodyType::kStatic) {
				continue;
			}
			//Dynamicのときだけ押す
			if (pairA->bodyType == BodyType::kDynamic && pairB->bodyType == BodyType::kStatic) {
				Resolve(*pairA->owner, *pairB->owner, hitInfo);
			} else if (pairA->bodyType == BodyType::kStatic && pairB->bodyType == BodyType::kDynamic) {
				Resolve(*pairB->owner, *pairA->owner, hitInfo);
			} else {
				//両方Dynamicはとりあえず片方だけ押す
				Resolve(*pairA->owner, *pairB->owner, hitInfo);
			}
		}
	}
}

//衝突判定を行えるか
bool ColliderManager::IsActive(Collider* collider) {
	if (!collider) {
		//colliderがnullだったら衝突判定を行えない
		return false;
	} else if (!collider->isEnabled) {
		//colliderが無効化されていたら衝突判定を行えない
		return false;
	} else if (!collider->owner) {
		//colliderのownerがnullだったら衝突判定を行えない
		return false;
	}
	return true;
}

//押し出し
void ColliderManager::Resolve(ColliderState& self, const ColliderState& other, HitInfo hit) {
	Vector3 pos = *self.translatePtr;
	Vector3 vel = *self.velocityPtr;

	// normalを「other→self」方向に揃える
	Vector3 delta = pos - *other.translatePtr;
	if (delta.Dot(hit.normal) < 0.0f) {
		hit.normal = -hit.normal;
	}

	//押し戻し
	const float slop = 0.001f;
	pos += hit.normal * (hit.depth + slop);

	float vn = vel.Dot(hit.normal);
	if (vn < 0.0f) {
		vel -= hit.normal * vn;
	}

	bool isOnGround = false;
	//下に地面があるかどうか
	if (hit.normal.y > 0.5f) {
		isOnGround = true;
		vel.y = 0.0f;
	}

	//位置と速度をを設定
	*self.translatePtr = pos;
	*self.velocityPtr = vel;
	//地面の上にいるか
	*self.isOnGroundPtr = isOnGround;
}

//Layerを使った衝突判定
bool ColliderManager::IsLayerCollidable(const Collider* self, const Collider* other) {
	return
		(self->maskLayer & static_cast<uint32_t>(other->layer)) != 0 &&
		(other->maskLayer & static_cast<uint32_t>(self->layer)) != 0;
}
