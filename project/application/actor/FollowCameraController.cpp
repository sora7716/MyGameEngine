#define NOMIMNAX
#include "FollowCameraController.h"
#include "MathUtility.h"
#include "MatrixUtility.h"
#include "GameObject.h"
#include <cmath>
#include <algorithm>

//コンストラクタ
FollowCameraController::FollowCameraController(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
FollowCameraController::~FollowCameraController() {
}

//初期化
void FollowCameraController::Initialize() {
	spherical_.radius = 6.0f;
	spherical_.theta = 0.0f;
	spherical_.phi = -mathUtility::kPi / 2.0f;
}

//更新
void FollowCameraController::Update() {
	//ターゲットがなければ
	if(!target_) {
		return;
	}

	//追従する速度の制限
	followSpeed_ = std::max(followSpeed_,0.0f);

	//球面座標系に制限を設ける
	const float limit = mathUtility::kPi / 2.0f - mathUtility::kEpsilon;
	spherical_.radius = std::max(spherical_.radius,0.1f);
	spherical_.theta = std::clamp(spherical_.theta,-limit,limit);
	//カメラのワールド行列
	Matrix4x4 cameraMatrix = MakeCameraMatrix();

	//目標の位置
	Transform& transform = GetOwner()->GetTransform();
	Transform targetTransform = matrixUtility::DecomposeMatrix(cameraMatrix,transform.scale);

	//目標の位置へ補間
	float t = 1.0f - std::exp(-followSpeed_ * mathUtility::GetDeltaTime());
	transform = Transform::Lerp(transform,targetTransform,t);
}

//複製
std::unique_ptr<Component> FollowCameraController::Clone(GameObject* gameObject) const {
	std::unique_ptr<FollowCameraController>cloneInstance = std::make_unique<FollowCameraController>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string FollowCameraController::GetTypeName() const {
	return "FollowCameraController";
}

//プロパティ一覧を登録
void FollowCameraController::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("動径",spherical_.radius);
	registry->Add("仰角",spherical_.theta);
	registry->Add("方位角",spherical_.phi);
	registry->Add("追従する速度",followSpeed_);
}

//ターゲットの設定
void FollowCameraController::SetTarget(GameObject* target) {
	target_ = target;
	//カメラのワールド行列
	Matrix4x4 cameraMatrix = MakeCameraMatrix();
	//座標の初期化
	Transform& transform = GetOwner()->GetTransform();
	transform = matrixUtility::DecomposeMatrix(cameraMatrix,transform.scale);
}

//動径の設定
void FollowCameraController::SetRadius(float radius) {
	spherical_.radius = radius;
}

//仰角の設定
void FollowCameraController::SetTheta(float theta) {
	spherical_.theta = theta;
}

//方位角の設定
void FollowCameraController::SetPhi(float phi) {
	spherical_.phi = phi;
}

//球面座標の設定
void FollowCameraController::SetSpherical(const Spherical& spherical) {
	spherical_ = spherical;
}

//追従する速度を設定
void FollowCameraController::SetFollowSpeed(float followSpeed) {
	followSpeed_ = followSpeed;
}

//カメラ行列の作成
Matrix4x4 FollowCameraController::MakeCameraMatrix() {
	//オフセットを求める
	Vector3 offset = mathUtility::ToCartesian(spherical_);
	//視線の位置
	Vector3 eye = target_->GetWorldPos() + offset;

	//世界の上
	Vector3 worldUp = Vector3::GetUp();
	//カメラの前方
	Vector3 forward = (target_->GetWorldPos() - eye).Normalize();
	//カメラの右
	Vector3 right = worldUp.Cross(forward).Normalize();
	//カメラの上
	Vector3 up = forward.Cross(right);

	//カメラのワールド行列
	Matrix4x4 cameraMatrix = {{
		{right.x,right.y,right.z,0.0f},
		{up.x,up.y,up.z,0.0f},
		{forward.x,forward.y,forward.z,0.0f},
		{eye.x,eye.y,eye.z,1.0f},
		}
	};

	return cameraMatrix;
}
