#include "DebugCamera.h"
#include "Input.h"
#include "algorithm/Math.h"
#include "CameraManager.h"
#include "ImGuiManager.h"
#include <algorithm>

//初期化
void DebugCamera::Initialize(Input* input, CameraManager* cameraManager) {
	//入力
	input_ =input;

	//カメラ
	camera_ = cameraManager->FindCamera("debugCamera");

	//fovYの設定
	fovY_ = camera_->GetFovY();
}

//更新
void DebugCamera::Update() {
	//デバッグするかどうか
	if (isDebug) {
		//平行移動の更新
		TranslateUpdate();

		//回転の操作
		RotateControl();

		//ズーム操作
		ZoomControl();
	}

	//デバッグOn/Off
	if (input_->TriggerKey(DIK_ESCAPE) || input_->TriggerXboxPad(xBoxPadNumber_, XboxInput::kStart)) {
		isDebug = !isDebug;
	}

	//カメラ
	camera_->SetQuaternion(rotate_);
	camera_->SetTranslate(translate_);
	camera_->Update();
}

//カメラのゲッター
Camera* DebugCamera::GetCamera() {
	return camera_;
}

//デバッグ中どうかのフラグのゲッター
const bool DebugCamera::IsDebug()const {
	return isDebug;
}

//XboxPadの番号のセッター
void DebugCamera::SetXBoxPadNumber(DWORD xboxPadNumber) {
	xBoxPadNumber_ = xboxPadNumber;
}

//デバックに使用する
void DebugCamera::Debug() {
#ifdef USE_IMGUI
	ImGui::Text("ESCAPE or XboxPadforStart");
	ImGui::Text("DebugMode:%s", isDebug ? "ON" : "OFF");
	ImGui::DragFloat4("rotate", &rotate_.x, 0.1f);
	ImGui::DragFloat2("flick", &mouseFlick_.x, 0.1f);
	ImGui::DragFloat("fovY", &fovY_, 0.1f);
#endif // USE_IMGUI
}

//左右移動の操作
void DebugCamera::StrafeControl() {
	//横移動
	if (input_->PressKey(DIK_A)) {
		moveDir_.x = -1.0f;
	} else if (input_->PressKey(DIK_D)) {
		moveDir_.x = 1.0f;
	} else {
		moveDir_.x = 0.0f;
	}
}

//上下移動の操作
void DebugCamera::ElevateControl() {
	if (input_->PressKey(DIK_Q)) {
		moveDir_.y = -1.0f;
	} else if (input_->PressKey(DIK_E)) {
		moveDir_.y = 1.0f;
	} else if (input_->PressXboxPad(xBoxPadNumber_, XboxInput::kLT)) {
		moveDir_.y = -1.0f;
	} else if (input_->PressXboxPad(xBoxPadNumber_, XboxInput::kRT)) {
		moveDir_.y = 1.0f;
	} else {
		moveDir_.y = 0.0f;
	}
}

//前後移動の操作
void DebugCamera::DollyControl() {
	if (input_->PressKey(DIK_W)) {
		moveDir_.z = 1.0f;
	} else if (input_->PressKey(DIK_S)) {
		moveDir_.z = -1.0f;
	} else {
		moveDir_.z = 0.0f;
	}

}

//ズーム操作
void DebugCamera::ZoomControl() {
	//マウスホイールの回転量でズームイン、ズームアウト
	fovY_ -= static_cast<float>(input_->GetWheelRotate()) * kZoomSpeedMagnification;

	//パッドの操作
	if (input_->PressXboxPad(xBoxPadNumber_, XboxInput::kDPadUp)) {
		fovY_ -= kXboxPadZoomSpeed;
	} else if (input_->PressXboxPad(xBoxPadNumber_, XboxInput::kDPadDown)) {
		fovY_ += kXboxPadZoomSpeed;
	}

	//fovYを範囲内で止める
	fovY_ = std::clamp(fovY_, kMinFovY, kMaxFovY);
	//fovYのセット
	camera_->SetFovY(fovY_);

	//ズーム操作のリセット
	if (input_->PressMouseButton(Click::kMiddle) || input_->PressXboxPad(xBoxPadNumber_, XboxInput::kRTHUMB)) {
		fovY_ = 0.45f;
	}
}

//回転の操作
void DebugCamera::RotateControl() {
	//マウスのフリックを取得
	if (input_->PressMouseButton(Click::kRight)) {
		Vector2Int mouseFlick = input_->GetMouseMoveAmount();
		//フリックの値をVector2に格納
		mouseFlick_.x = static_cast<float>(mouseFlick.x);
		mouseFlick_.y = static_cast<float>(mouseFlick.y);

		//フリックの値をカメラの回転に反映
		rotate_.x += mouseFlick_.y * kLookRadPerCount;
		rotate_.y += mouseFlick_.x * kLookRadPerCount;
	}

	//Xboxの右スティックの入力
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {
		Vector2 dir = input_->GetXboxPadRighttStick(xBoxPadNumber_);
		rotate_.x -= dir.Normalize().y * kXboxPadRotSpeed;
		rotate_.y += dir.Normalize().x * kXboxPadRotSpeed;
	}
}

//平行移動の更新
void DebugCamera::TranslateUpdate() {
	//X軸方向の移動
	StrafeControl();

	//Y軸方向の移動
	ElevateControl();

	//Z軸方向の移動
	DollyControl();

	//XboxPadの平行移動
	if (input_->IsXboxPadConnected(xBoxPadNumber_)) {
		if (std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).x) > 0.0f
			|| std::fabs(input_->GetXboxPadLeftStick(xBoxPadNumber_).y) > 0.0f) {
			moveDir_.x = input_->GetXboxPadLeftStick(xBoxPadNumber_).x;
			moveDir_.z = input_->GetXboxPadLeftStick(xBoxPadNumber_).y;
		}
	}

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateMatrix(Rendering::MakeRotateQuaternion(rotate_));

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = Math::TransformNormal(Vector3(moveDir_.x, 0.0f, moveDir_.z), rotMat);

	//Y軸のそのまま
	moveDir_ = { moveDirXZ.x,moveDir_.y,moveDirXZ.z };

	//カメラを移動させる
	translate_ += moveDir_.Normalize() * kMoveSpeed;
}