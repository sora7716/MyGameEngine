#include "OrbitCameraController.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "Input.h"

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

	constexpr float kPitchLimit = mathUtility::kPi / 2.0f - 0.01f;

	pitch_ = std::clamp(pitch_, -kPitchLimit, kPitchLimit);

	//カメラの向いている方向を取得
	Quaternion rotation = Quaternion::MakeQuaternionForEulerAngle({ pitch_,yaw_,0.0f });

	//回転後のカメラの相対位置
	Vector3 rotatedCameraPos = rotation.RotateVector({ 0.0f,0.0f,-distance_ });

	//カメラの位置
	gameObject_->GetTransform().translate = targetPos + rotatedCameraPos;

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

	//ピッチとヨーに適応
	pitch_ = mouseDelta.x * sensitivity_.x;
	yaw_ = mouseDelta.y * sensitivity_.y;
}
