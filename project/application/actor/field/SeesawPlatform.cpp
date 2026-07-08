#include "SeesawPlatform.h"
#include "algorithms/Rendering.h"

//コンストラクタ
SeesawPlatform::SeesawPlatform() {
}

//デストラクタ
SeesawPlatform::~SeesawPlatform() {
}

//初期化
void SeesawPlatform::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//オブジェクトの数とモデルを設定
	entityGroup_.objectCount = 1;
	entityGroup_.modelName = "wall";
	//初期化
	BaseGround::Initialize(object3dCommon, camera);
}

//更新
void SeesawPlatform::Update() {
	float leftWeight = 0.0f;
	float rightWeight = 0.0f;
	Vector3 platformPos = entityGroup_.entity[collisionBlockIndex_].gameObject.transform.translate;

	if (playerPos_.x < platformPos.x) {
		leftWeight += 1.0f;
	} else {
		rightWeight += 1.0f;
	}

	float targetAngle = (leftWeight - rightWeight) * 0.1f;

	// じわっと近づける
	seesawAngle_ += (targetAngle - seesawAngle_) * 0.1f;

	entityGroup_.entity[collisionBlockIndex_].gameObject.transform.quaternion =
		Rendering::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, seesawAngle_);	//更新
	BaseGround::Update();
}

//衝突したら
void SeesawPlatform::OnCollision(uint32_t index, ColliderState* other) {
	if (*other->tagPtr == Tag::kPlayer) {
		playerPos_ = *other->translatePtr;
		collisionBlockIndex_ = index;
	}
}
