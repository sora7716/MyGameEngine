#define NOMINMAX
#include "OrbitCameraController.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "Input.h"
#include <algorithm>

//コンストラクタ
OrbitCameraController::OrbitCameraController(GameObject* gameObject, Input& input) :Component(gameObject), input_(input){
}

//デストラクタ
OrbitCameraController::~OrbitCameraController(){
}

//初期化
void OrbitCameraController::Initialize(){
	gameObject_ = GetOwner();
}

//更新
void OrbitCameraController::Update(){
	//回転前のカメラの相対位置	
	Vector3 targetPos = target_->GetTransform().translate + targetOffset_;

	//カメラの回転
	ViewRotationControl();

	//ピッチを制限
	float minPitch = std::asin((1.0f - targetPos.y) / distance_);
	float maxPitch = 70.0f * mathUtility::kRad;
	pitch_ = std::clamp(pitch_, minPitch, maxPitch);

	//カメラの向いている方向を取得
	Quaternion rotation = Quaternion::MakeQuaternionForEulerAngle({ pitch_,yaw_,0.0f });

	//回転後のカメラの相対位置
	Vector3 rotatedCameraPos = rotation.RotateVector({ 0.0f,0.0f,-distance_ });

	//カメラの位置
	gameObject_->GetTransform().translate = targetPos + rotatedCameraPos;
	gameObject_->GetTransform().translate.y = std::max(gameObject_->GetTransform().translate.y, 1.0f);

	//回転を設定
	gameObject_->GetTransform().SetRotate(rotation);

}

//複製
std::unique_ptr<Component> OrbitCameraController::Clone(GameObject* gameObject) const{
	std::unique_ptr<OrbitCameraController>cloneInstance = std::make_unique<OrbitCameraController>(gameObject, this->input_);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//対象の設定
void OrbitCameraController::SetTarget(GameObject* target){
	target_ = target;
}

//カメラの回転に関する操作
void OrbitCameraController::ViewRotationControl(){
	//マウスの移動量の取得
	Vector2 mouseDelta = input_.GetMouseMoveAmount();

	//デッドゾーンを考慮する
	if (std::fabs(deadZone_.x) > mouseDelta.x && std::fabs(deadZone_.y) > mouseDelta.y){
		return;
	}

	//ピッチとヨーに適応
	pitch_ += mouseDelta.y * sensitivity_.y;
	yaw_ -= mouseDelta.x * sensitivity_.x;
}
