#include "GameCamera.h"
#include "Camera.h"
#include "ImGuiManager.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
#include "Input.h"

//初期化
void GameCamera::Initialize(Input* input, Camera* camera) {
	//入力の記録
	input_ = input;
	//カメラの記録
	camera_ = camera;
}

//更新
void GameCamera::Update() {
	//オフセットを設定
	offset_ = { 0.0f,2.0f,-15.0f };
	//カメラの回転
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {
		float rx = input_->GetXboxPadRighttStick(xBoxPadNumber_).y;
		float ry = input_->GetXboxPadRighttStick(xBoxPadNumber_).x;

		// デッドゾーン
		if (std::fabs(rx) < 0.15f) {
			rx = 0.0f;
		}
		if (std::fabs(ry) < 0.15f) {
			ry = 0.0f;
		}

		// 回転更新（dtも掛けるのが理想）
		eulerAngle_.y += ry * kRotateSpeed * MathUtility::kDeltaTime * -1.0f;
		eulerAngle_.x += rx * kRotateSpeed * MathUtility::kDeltaTime * -1.0f;

		//X軸制限（0〜90度）
		float minX = -10.0f * MathUtility::kRad;
		float maxX = 60.0f * MathUtility::kRad;

		eulerAngle_.x = std::clamp(eulerAngle_.x, minX, maxX);
	}

	//カメラの回転を設定
	camera_->SetQuaternion(Quaternion::MakeQuaternionForEulerAngle(eulerAngle_));

	//カメラの角度から回転行列を求める
	Matrix4x4 rotMat = MatrixUtility::MakeRotateMatrix(Quaternion::MakeQuaternionForEulerAngle(eulerAngle_));

	//オフセットをカメラの回転に合わせて回転させる
	offset_ = MathUtility::TransformNormal(offset_, rotMat);

	//カメラの位置をオフセット分離す
	camera_->SetTranslate(targetPos_ + offset_);

	//カメラの更新
	camera_->Update();
}

//デバッグ
void GameCamera::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("rotate", &eulerAngle_.x, 0.1f);
	ImGui::DragFloat3("offset", &offset_.x, 0.1f);
#endif // USE_IMGUI
}

//カメラのゲッター
Camera* GameCamera::GetCamera() {
	return camera_;
}

//カメラのセッター
void GameCamera::SetCamera(Camera* camera) {
	camera_ = camera;
}

//ターゲットの位置のセッター
void GameCamera::SetTargetPos(const Vector3& targetPos) {
	targetPos_ = targetPos;
}
