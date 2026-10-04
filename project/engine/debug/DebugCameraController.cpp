#include "DebugCameraController.h"
#include "MathUtility.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "MatrixUtility.h"
#include <algorithm>

//コンストラクタ
DebugCameraController::DebugCameraController(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
DebugCameraController::~DebugCameraController(){
}

//初期化
void DebugCameraController::Initialize(){
	//基底クラスの初期化
	Component::Initialize();
	gameObject_ = GetOwner();
	pitch_ = 0.35f;
	yaw_ = 0.04f;
	gameObject_->GetTransform().SetEulerAngle({ pitch_,yaw_,0.0f });
	gameObject_->GetTransform().translate = { -2.9f,11.0f,-29.0f };
	camera_ = gameObject_->GetComponent<Camera>();

	assert(camera_);

	fovY_ = camera_->GetFovY();

}

//更新
void DebugCameraController::Update(){
	//基底クラスの更新
	Component::Update();

	//デバッグモードがfalseだった場合
	if (!isControlEnabled_){
		return;
	}

	//平行移動の更新
	TranslateUpdate();

	//回転の操作
	RotateControl();

	//ズーム操作
	ZoomControl();
}

//更新のフェーズの取得
UpdatePhase DebugCameraController::GetUpdatePhase(){
	return UpdatePhase::kDebug;
}

//複製
std::unique_ptr<Component> DebugCameraController::Clone(GameObject* gameObject) const{
	std::unique_ptr<DebugCameraController>cloneInstance = std::make_unique<DebugCameraController>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//DebugCameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->fovY_ = this->fovY_;
	return cloneInstance;
}

//デバッグが有効かの設定
void DebugCameraController::SetIsDebugControlEnabled(bool isControlEnabled){
	isControlEnabled_ = isControlEnabled;
}

//左右移動の操作
void DebugCameraController::StrafeControl(){
#ifdef USE_IMGUI
	//横移動
	if (ImGui::IsKeyDown(ImGuiKey_A)){
		moveDir_.x = -1.0f;
	} else if (ImGui::IsKeyDown(ImGuiKey_D)){
		moveDir_.x = 1.0f;
	} else{
		moveDir_.x = 0.0f;
	}
#endif // USE_IMGUI
}

//上下移動の操作
void DebugCameraController::ElevateControl(){
#ifdef USE_IMGUI
	if (ImGui::IsKeyDown(ImGuiKey_Q)){
		moveDir_.y = -1.0f;
	} else if (ImGui::IsKeyDown(ImGuiKey_E)){
		moveDir_.y = 1.0f;
	} else{
		moveDir_.y = 0.0f;
	}
#endif // USE_IMGUI
}

//前後移動の操作
void DebugCameraController::DollyControl(){
#ifdef USE_IMGUI
	if (ImGui::IsKeyDown(ImGuiKey_W)){
		moveDir_.z = 1.0f;
	} else if (ImGui::IsKeyDown(ImGuiKey_S)){
		moveDir_.z = -1.0f;
	} else{
		moveDir_.z = 0.0f;
	}
#endif // USE_IMGUI
}

//ズーム操作
void DebugCameraController::ZoomControl(){
#ifdef USE_IMGUI
	//マウスホイールの回転量でズームイン、ズームアウト
	fovY_ -= ImGui::GetIO().MouseWheel * kZoomSpeedMagnification;
	//ズーム操作のリセット
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)){
		fovY_ = 0.45f;
	}
	//fovYを範囲内で止める
	fovY_ = std::clamp(fovY_, kMinFovY, kMaxFovY);

	//fovYのセット
	camera_->SetFovY(fovY_);
#endif // USE_IMGUI
}

//回転の操作
void DebugCameraController::RotateControl(){
#ifdef USE_IMGUI
	//マウスのフリックを取得
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)){
		ImVec2 mouseFlick = ImGui::GetIO().MouseDelta;

		//フリックの値をカメラの回転に反映
		pitch_ += mouseFlick.y * kLookRadPerCount;
		yaw_ += mouseFlick.x * kLookRadPerCount;

		constexpr float kPitchLimit = mathUtility::kPi / 2.0f - 0.01f;

		pitch_ = std::clamp(pitch_, -kPitchLimit, kPitchLimit);
		gameObject_->GetTransform().SetEulerAngle({ pitch_,yaw_,0.0f });
	}
#endif // USE_IMGUI
}

//平行移動の更新
void DebugCameraController::TranslateUpdate(){
#ifdef USE_IMGUI
#endif // USE_IMGUI
	//X軸方向の移動
	StrafeControl();

	//Y軸方向の移動
	ElevateControl();

	//Z軸方向の移動
	DollyControl();

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = matrixUtility::MakeRotateMatrix(gameObject_->GetTransform().rotate);

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = mathUtility::TransformNormal(Vector3(moveDir_.x, 0.0f, moveDir_.z), rotMat);

	//Y軸のそのまま
	moveDir_ = { moveDirXZ.x,moveDir_.y,moveDirXZ.z };

	//カメラを移動させる
	gameObject_->GetTransform().translate += moveDir_.Normalize() * kMoveSpeed;
}