#include "DebugCamera.h"
#include "MathUtility.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "Quaternion.h" 
#include <algorithm>

//初期化
void DebugCamera::Initialize(Camera* debugCamera){
	//カメラ
	camera_ = debugCamera;

	//fovYの設定
	fovY_ = camera_->GetFovY();
}

//更新
void DebugCamera::Update(){
#ifdef USE_IMGUI
	//エスケープキーを入力したら
	if (ImGui::IsKeyPressed(ImGuiKey_Escape)){
		isDebug_ = !isDebug_;
	}
#endif // USE_IMGUI

	//デバッグモードがfalseだった場合
	if (!isDebug_){
		return;
	}

	//平行移動の更新
	TranslateUpdate();

	//回転の操作
	RotateControl();

	//ズーム操作
	ZoomControl();

	//カメラ
	camera_->SetQuaternion(Quaternion::MakeQuaternionForEulerAngle(rotate_));
	camera_->SetTranslate(translate_);
}

//カメラのゲッター
Camera* DebugCamera::GetCamera(){
	return camera_;
}

//デバックに使用する
void DebugCamera::Debug(){
#ifdef USE_IMGUI
	ImGui::DragFloat4("rotate", &rotate_.x, 0.1f);
	ImGui::DragFloat2("flick", &mouseFlick_.x, 0.1f);
	ImGui::DragFloat("fovY", &fovY_, 0.1f);
#endif // USE_IMGUI
}

//左右移動の操作
void DebugCamera::StrafeControl(){
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
void DebugCamera::ElevateControl(){
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
void DebugCamera::DollyControl(){
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
void DebugCamera::ZoomControl(){
#ifdef USE_IMGUI
	//ImGuiを使用していた場合
	if (ImGui::GetIO().WantCaptureMouse){
		return;
	}

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
void DebugCamera::RotateControl(){
#ifdef USE_IMGUI
	//ImGuiを使用していた場合
	if (ImGui::GetIO().WantCaptureMouse){
		return;
	}

	//マウスのフリックを取得
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)){
		ImVec2 mouseFlick = ImGui::GetIO().MouseDelta;
		//フリックの値をVector2に格納
		mouseFlick_.x = mouseFlick.x;
		mouseFlick_.y = mouseFlick.y;

		//フリックの値をカメラの回転に反映
		rotate_.x += mouseFlick_.y * kLookRadPerCount;
		rotate_.y += mouseFlick_.x * kLookRadPerCount;
	}
#endif // USE_IMGUI
}

//平行移動の更新
void DebugCamera::TranslateUpdate(){
#ifdef USE_IMGUI
	//ImGuiを使用していた場合
	if (ImGui::GetIO().WantCaptureKeyboard){
		return;
	}
#endif // USE_IMGUI
	//X軸方向の移動
	StrafeControl();

	//Y軸方向の移動
	ElevateControl();

	//Z軸方向の移動
	DollyControl();

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = matrixUtility::MakeRotateMatrix(Quaternion::MakeQuaternionForEulerAngle(rotate_));

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = mathUtility::TransformNormal(Vector3(moveDir_.x, 0.0f, moveDir_.z), rotMat);

	//Y軸のそのまま
	moveDir_ = { moveDirXZ.x,moveDir_.y,moveDirXZ.z };

	//カメラを移動させる
	translate_ += moveDir_.Normalize() * kMoveSpeed;
}