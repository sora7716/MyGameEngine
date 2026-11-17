#include "CameraControlComponent.h"
#include "FollowCameraController.h"
#include <cassert>

//コンストラクタ
CameraControlComponent::CameraControlComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
CameraControlComponent::~CameraControlComponent() {
}

//セットアップ
void CameraControlComponent::Setup(FollowCameraController* followCamera,float followSpeed) {
	//追従カメラの記録
	assert(followCamera);
	followCamera_ = followCamera;
	//ピッチの記録
	pitch_ = followCamera_->GetTheta();
	//ヨーの記録
	yaw_ = followCamera_->GetPhi();
	//追従速度の記録
	followSpeed_ = followSpeed;
}

//初期化
void CameraControlComponent::Initialize() {
}

//更新
void CameraControlComponent::Update() {
	//追従カメラに値を送信
	followCamera_->SetTheta(pitch_);
	followCamera_->SetPhi(yaw_);
	followCamera_->SetFollowSpeed(followSpeed_);
}

//プロパティ一覧
void CameraControlComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("ピッチ",pitch_);
	registry->Add("ヨーー",yaw_);
	registry->Add("追従速度",followSpeed_);
}

//複製
std::unique_ptr<Component> CameraControlComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<CameraControlComponent>cloneInstance = std::make_unique<CameraControlComponent>(gameObject);
	cloneInstance->Setup(followCamera_,followSpeed_);
	cloneInstance->Initialize();
	cloneInstance->pitch_ = this->pitch_;
	cloneInstance->yaw_ = this->yaw_;
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string CameraControlComponent::GetTypeName() const {
	return "CameraControlComponent";
}

//ピッチの設定
void CameraControlComponent::SetPitch(float pitch) {
	pitch_ = pitch;
}

//ヨーの設定
void CameraControlComponent::SetYaw(float yaw) {
	yaw_ = yaw;
}
