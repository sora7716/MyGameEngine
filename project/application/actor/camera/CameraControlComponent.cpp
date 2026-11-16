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
	//追従速度の記録
	followSpeed_ = followSpeed;
}

//初期化
void CameraControlComponent::Initialize() {
}

//更新
void CameraControlComponent::Update() {
	//追従カメラに値を送信
	followCamera_->SetTheta(theta_);
	followCamera_->SetPhi(phi_);
	followCamera_->SetFollowSpeed(followSpeed_);
}

//プロパティ一覧
void CameraControlComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
}

//複製
std::unique_ptr<Component> CameraControlComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<CameraControlComponent>cloneInstance = std::make_unique<CameraControlComponent>(gameObject);
	cloneInstance->Setup(followCamera_,followSpeed_);
	cloneInstance->Initialize();
	cloneInstance->theta_ = this->theta_;
	cloneInstance->phi_ = this->phi_;
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string CameraControlComponent::GetTypeName() const {
	return "CameraControlComponent";
}

//仰角の設定
void CameraControlComponent::SetTheta(float theta) {
	theta_ = theta;
}

//方位角の設定
void CameraControlComponent::SetPhi(float phi) {
	phi_ = phi;
}
