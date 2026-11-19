#include "CameraControlComponent.h"
#include "FollowCameraController.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "Logger.h"
#include <cassert>
#include <algorithm>

//コンストラクタ
CameraControlComponent::CameraControlComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
CameraControlComponent::~CameraControlComponent() {
}

//セットアップ
void CameraControlComponent::Setup(float followSpeed) {
	//追従速度の記録
	followSpeed_ = followSpeed;
}

//初期化
void CameraControlComponent::Initialize() {
	//フォローカメラがあるか
	followCamera_ = GetOwner()->GetComponent<FollowCameraController>();
	if(!followCamera_) {
		Logger::OutputLog("CameraObjectにFollowCameraControllerがコンポーネントされていません");
		return;
	}

		//ピッチの記録
	pitch_ = followCamera_->GetTheta();
	//ヨーの記録
	yaw_ = followCamera_->GetPhi();
}

//更新
void CameraControlComponent::Update() {
	if(!followCamera_) {
		Logger::OutputLog("CameraObjectにFollowCameraControllerがコンポーネントされていません");
		return;
	}

	//追従カメラに値を送信
	pitch_ += axis_.y * rotSpeed_ * mathUtility::GetDeltaTime();
	yaw_ += axis_.x * rotSpeed_ * mathUtility::GetDeltaTime();
	
	//ピッチの最大値
	constexpr float kMaxPitch = 90.0f * mathUtility::kRad;
	//ピッチの最小値
	constexpr float kMinPitch = -90.0f * mathUtility::kRad;
	//ピッチを制限
	pitch_ = std::clamp(pitch_,kMinPitch,kMaxPitch);
	
	followCamera_->SetTheta(pitch_);
	followCamera_->SetPhi(yaw_);
	followCamera_->SetFollowSpeed(followSpeed_);
}

//プロパティ一覧
void CameraControlComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("回転速度",rotSpeed_);
	registry->Add("追従速度",followSpeed_);
}

//複製
std::unique_ptr<Component> CameraControlComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<CameraControlComponent>cloneInstance = std::make_unique<CameraControlComponent>(gameObject);
	cloneInstance->Setup(followSpeed_);
	cloneInstance->Initialize();
	cloneInstance->pitch_ = this->pitch_;
	cloneInstance->yaw_ = this->yaw_;
	cloneInstance->axis_ = this->axis_;
	cloneInstance->rotSpeed_ = this->rotSpeed_;
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string CameraControlComponent::GetTypeName() const {
	return "CameraControlComponent";
}

//軸を設定
void CameraControlComponent::SetAxis(const Vector2& axis) {
	axis_ = axis;
}
