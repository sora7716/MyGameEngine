#include "RigidBody.h"
#include "Physics.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "RenderingData.h"

//コンストラクタ
RigidBody::RigidBody(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
RigidBody::~RigidBody(){
}

//初期化
void RigidBody::Initialize(){
	//ゲームオブジェクトを記録
	gameObject_ = GetOwner();
}

//更新
void RigidBody::Update(){
	//重力の適応
	ApplyGravity();
	//加速度と速度を適応
	UpdateMotion();

	//加速度のリセット
	acceleration_ = {};
}

//複製
std::unique_ptr<Component> RigidBody::Clone(GameObject* gameObject) const{
	std::unique_ptr<RigidBody>cloneInstance = std::make_unique<RigidBody>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//速度を取得
Vector3& RigidBody::GetVelocity(){
	return velocity_;
}

//速度を取得
const Vector3& RigidBody::GetVelocity() const{
	return velocity_;
}

//加速度を取得
Vector3& RigidBody::GetAcceleration(){
	return acceleration_;
}

//加速度を取得
const Vector3& RigidBody::GetAcceleration() const{
	return acceleration_;
}

//速度と加速度を適応
void RigidBody::UpdateMotion(){
	Transform& transform = gameObject_->GetTransform();
	velocity_ += acceleration_ * mathUtility::kDeltaTime;
	transform.translate += velocity_ * mathUtility::kDeltaTime;
}

//重力を適応
void RigidBody::ApplyGravity(){
	acceleration_ += physics::kGravity;
}
