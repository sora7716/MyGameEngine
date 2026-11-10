#define NOMINMAX
#include "MovementComponent.h"
#include "RigidBody.h"
#include "MathUtility.h"
#include "MatrixUtility.h"
#include "GameObject.h"
#include <algorithm>
#include <cassert>

//コンストラクタ
MovementComponent::MovementComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
MovementComponent::~MovementComponent() {
}

//セットアップ
void MovementComponent::Setup(RigidBody* rigidBody,float moveSpeed,float moveResponse) {
	//リジッドボディの記録
	assert(rigidBody);
	rigidBody_ = rigidBody;
	//移動速度の記録
	moveSpeed_ = moveSpeed;
	//移動する際のレスポンスを設定
	moveResponse_ = std::max(moveResponse,1.0f);

}

//初期化
void MovementComponent::Initialize() {
}

//更新
void MovementComponent::Update() {
	//速度
	Vector3 targetVelocity = {};
	//方向ベクトルがゼロベクトルじゃない場合
	if(direction_.LengthSquared() != 0.0f) {
		//カメラの角度をもとに回転行列を求める
		Matrix4x4 rotMat = Matrix4x4::Identity4x4();
		if(cameraObject_) {
			rotMat = matrixUtility::MakeRotateMatrix(cameraObject_->GetTransform().rotate);
		}

		//カメラの向いてる方向を正にする(XとZ軸限定)
		Vector3 worldDirection = mathUtility::TransformNormal(direction_,rotMat);
		//Y軸は考えない
		worldDirection.y = 0.0f;

		//長さが0より大きければ
		if(worldDirection.LengthSquared() > 0.0f) {
			worldDirection = worldDirection.Normalize();
		}

		//目標速度を決定
		targetVelocity = worldDirection * moveSpeed_;

		//方向ベクトルをリセット
		direction_ = {};
	} else {
		targetVelocity = {};
	}

	//水平移動の加速度を目標速度に近づける
	Vector3& velocity = rigidBody_->GetVelocity();
	Vector3& acceleration = rigidBody_->GetAcceleration();
	acceleration.x = (targetVelocity.x - velocity.x) * moveResponse_;
	acceleration.z = (targetVelocity.z - velocity.z) * moveResponse_;
}

//プロパティ一覧
void MovementComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("moveSpeed",moveSpeed_);
	registry->Add("moveResponse",moveResponse_);
}

//複製
std::unique_ptr<Component> MovementComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<MovementComponent>cloneInstance = std::make_unique<MovementComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->rigidBody_ = this->rigidBody_;
	cloneInstance->cameraObject_ = this->cameraObject_;
	cloneInstance->moveSpeed_ = this->moveSpeed_;
	cloneInstance->moveResponse_ = this->moveResponse_;
	cloneInstance->direction_ = this->direction_;
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string MovementComponent::GetTypeName() const {
	return "MovementComponent";
}

//方向ベクトルの追加
void MovementComponent::AddDirection(const Vector3& direction) {
	direction_ += direction;
}

//カメラオブジェクトの設定
void MovementComponent::SetCameraObject(GameObject* cameraObject) {
	cameraObject_ = cameraObject;
}
