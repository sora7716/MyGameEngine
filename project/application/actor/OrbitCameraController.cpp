#define NOMINMAX
#include "OrbitCameraController.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "BaseScene.h"
#include "Input.h"
#include <algorithm>

//コンストラクタ
OrbitCameraController::OrbitCameraController(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
OrbitCameraController::~OrbitCameraController(){
}

//初期化
void OrbitCameraController::Initialize(){
	//ゲームオブジェクトの取得
	gameObject_ = GetOwner();
	//現在リンクされているシーンを取得
	BaseScene* currentScene = gameObject_->GetCurrentScene();
	//入力の取得
	input_ = currentScene->GetSceneContext().input;
}

//更新
void OrbitCameraController::Update(){
	//エスケープキーが押されたら
	if (input_->TriggerKey(DIK_ESCAPE)){
		isMovingCamera_ = !isMovingCamera_;
	}

	//カメラを動か差ない場合
	if (!isMovingCamera_){
		return;
	}

	//ターゲットがいなければ
	if (!target_){
		return;
	}

	//回転前のカメラの相対位置	
	Vector3 targetPos = target_->GetTransform().translate + targetOffset_;

	//カメラの回転
	ViewRotationControl();

	//ピッチを制限
	float minPitch = std::asin((1.0f - targetPos.y) / distance_);
	float maxPitch = 70.0f * mathUtility::kRad;
	pitch_ = std::clamp(pitch_, minPitch, maxPitch);

	//カメラの向いている方向を取得
	Quaternion rotation = Quaternion::EulerAngleToQuaternion({ pitch_,yaw_,0.0f });

	//回転後のカメラの相対位置
	Vector3 rotatedCameraPos = rotation.RotateVector({ 0.0f,0.0f,-distance_ });

	//カメラの位置
	gameObject_->GetTransform().translate = targetPos + rotatedCameraPos;

	//回転を設定
	gameObject_->GetTransform().SetRotate(rotation);

}

//複製
std::unique_ptr<Component> OrbitCameraController::Clone(GameObject* gameObject) const{
	std::unique_ptr<OrbitCameraController>cloneInstance = std::make_unique<OrbitCameraController>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//ゲームオブジェクトから解除する
void OrbitCameraController::OnGameObjectRemoving(GameObject* target){
	if (target_ == target){
		target_ = nullptr;
	}
}

//対象の設定
void OrbitCameraController::SetTarget(GameObject* target){
	target_ = target;
}

//カメラの回転に関する操作
void OrbitCameraController::ViewRotationControl(){
	//マウスの移動量の取得
	Vector2 mouseDelta = input_->GetMouseMoveAmount();

	//デッドゾーンを考慮する
	if (deadZone_.x > std::fabs(mouseDelta.x) && deadZone_.y > std::fabs(mouseDelta.y)){
		return;
	}

	//ピッチとヨーに適応
	pitch_ += mouseDelta.y * sensitivity_.y;
	yaw_ -= mouseDelta.x * sensitivity_.x;
}
