#include "ColliderData.h"

//初期化
void ColliderState::Initialize(GameObject& gameObject, PhysicsData& physicsData, Vector3& scale) {
	scalePtr = &scale;
	rotatePtr = &gameObject.transformData.quaternion;
	translatePtr = &gameObject.transformData.translate;
	velocityPtr = &physicsData.velociy;
	isOnGroundPtr = &physicsData.isOnGround;
}
