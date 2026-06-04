#include "ColliderData.h"

//初期化
void ColliderState::Initialize(GameObject& gameObject, PhysicsData& physicsData, Vector3& scale) {
	scalePtr = &scale;
	rotatePtr = &gameObject.transform.quaternion;
	translatePtr = &gameObject.transform.translate;
	velocityPtr = &physicsData.velocity;
	isOnGroundPtr = &physicsData.isOnGround;
	tagPtr = &gameObject.tag;
}
