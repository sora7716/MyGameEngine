#include "ActorData.h"
#include "Object3d.h"
#include "WireframeObject3d.h"

//ゲームオブジェクトの初期化
void GameObject::Initialize() {
	transformData = { Vector3::MakeAllOne(),{},{} };
	velocity = {};
	acceleration = {};
	direction = {};
	isAlive = true;
	isOnGround = false;
}

//レンダーオブジェクトのobject3dとhitBoxの生成
RenderObject& RenderObject::Create() {
	//Object3dの生成
	object3d = std::make_unique<Object3d>();
	//hitBoxの生成
	hitBox = std::make_unique<WireframeObject3d>();

	return *this;
}

//レンダーオブジェクトのmaterialの初期化
RenderObject& RenderObject::InitializeMaterial() {
	//色を設定
	material.color = Vector4::MakeWhiteColor();
	//ライティングを無し
	material.enableLighting = true;
	//光沢度
	material.shininess = 10.0f;
	//uvMatrixの初期化
	material.uvMatrix = Matrix4x4::Identity4x4();

	return *this;
}

//レンダーオブジェクトを作成
RenderObject&& RenderObject::Build() {
	return std::move(*this);
}

//コライダーの状態の初期化
void ColliderState::Initialize(Vector3& hitBoxScale, GameObject& gameObject, Matrix4x4& worldMatrix, Tag tag) {
	scalePtr = &hitBoxScale;
	rotatePtr = &gameObject.transformData.rotate;
	translatePtr = &gameObject.transformData.translate;
	velocityPtr = &gameObject.velocity;
	worldMatrixPtr = &worldMatrix;
	isOnGroundPtr = &gameObject.isOnGround;
	this->tag = tag;
}

//コライダーのownerのセッター
Collider& Collider::SetOwner(ColliderState* colliderState) {
	owner = colliderState;
	return *this;
}

Collider& Collider::CreateObb() {
	return *this;
}

//コライダーのBodyTypeのセッター
Collider& Collider::SetBodyType(BodyType bodyType) {
	this->bodyType = bodyType;
	return *this;
}

//コライダーのisTriggerのセッター
Collider& Collider::SetIsTrigger(bool isTrigger) {
	this->isTrigger = isTrigger;
	return *this;
}

//コライダーのisEnableのセッター
Collider& Collider::SetIsEnebled(bool isEnabled) {
	this->isEnabled = isEnabled;
	return *this;
}

//コライダーのレイヤーのセッター
Collider& Collider::SetLayer(Layer layer) {
	this->layer = layer;
	return *this;
}

//コライダーのマスクレイヤーのセッター
Collider& Collider::SetMaskLayer(uint32_t maskLayer) {
	this->maskLayer = maskLayer;
	return *this;
}

Collider& Collider::SetOnCollision(std::function<void(ColliderState* other)> onCollision) {
	this->onCollision = onCollision;
	return *this;
}

Collider&& Collider::Build() {
	return std::move(*this);
}

uint32_t ToBits(Layer layer) {
	return static_cast<uint32_t>(layer);
}
