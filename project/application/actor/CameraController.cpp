#include "CameraController.h"
#include "engine/camera/Camera.h"
#include "application/actor/Player.h"
#include "engine/math/RenderingData.h"
#include "engine/debug/GlobalVariables.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/3d/Object3d.h"
#include <algorithm>

//初期化
void CameraController::Initialize(Camera* camera) {
	//カメラを受け取る
	camera_ = camera;
	//角度
	quaternion_ = { 0.51f,0.0f,0.0f };
	//位置のオフセット
	targetOffset_ = { 0.0f,3.0f,-8.0f };
	//FovY
	fovY_ = 1.0f;

	//調整項目を登録
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//項目を生成
	globalVariables->CreateGroup(groupName_);
	//各調整項目を登録
	//globalVariables->AddItem(groupName_, "rotate", cameraRotate_);
	globalVariables->AddItem(groupName_, "offset", targetOffset_);
	globalVariables->AddItem(groupName_, "fovY", fovY_);
}

//更新
void CameraController::Update() {
	//調整項目を適応
	ApplyGlobalVariables();
	//追従対象のトランスフォームデータを参照
	TransformData targetTransformData = target_->GetTransformData();
	//追従対象とオフセットと追従対象の速度から目的地を計算
	goalPosition_ = targetTransformData.translate + targetOffset_ + target_->GetVelocity() * kVelocityBias;

	//座標補間によりゆったり追従
	Vector3 cameraTranslate = Vector3::Lerp(camera_->GetTranslate(), goalPosition_, kInterpolationRate);

	//追従対象が画面外に出ないように補正
	cameraTranslate.x = std::clamp(cameraTranslate.x, target_->GetTransformData().translate.x + margin.left, target_->GetTransformData().translate.x + margin.right);
	cameraTranslate.y = targetOffset_.y;
	cameraTranslate.z = std::clamp(cameraTranslate.z, target_->GetTransformData().translate.z + margin.back, target_->GetTransformData().translate.z + margin.front);

	//移動範囲の制限
	cameraTranslate.x = std::clamp(cameraTranslate.x, movableArea_.left, movableArea_.right);
	cameraTranslate.z = std::clamp(cameraTranslate.z, movableArea_.back, movableArea_.front);
	//制限した移動範囲を設定
	camera_->SetTranslate(cameraTranslate);

	//カメラの角度のセッター
	camera_->SetQuaternion(quaternion_);

	//FovYのセッター
	camera_->SetFovY(fovY_);

	//行列を更新
	camera_->Update();
}

//リセット
void CameraController::Reset() {
	//追従対象のトランスフォームデータを参照
	TransformData targetTransformData = target_->GetTransformData();
	//追従対象とオフセットからカメラの座標を計算
	camera_->SetTranslate(targetTransformData.translate + targetOffset_);
}

//追従対象
void CameraController::SetTarget(Player* target) {
	target_ = target;
}

//移動範囲のセッター
void CameraController::SetMovableArea(const Rect& area) {
	movableArea_ = area;
}

//オフセットのセッター
void CameraController::SetOffset(const Vector3& offset) {
	targetOffset_ = offset;
}

//回転のセッター
void CameraController::SetRotate(const Quaternion& rotate) {
	quaternion_ = rotate;
}

//調整項目を適応
void CameraController::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	fovY_ = globalVariables->GetValue<float>(groupName_, "fovY");
	//cameraRotate_ = globalVariables->GetValue<Vector3>(groupName_, "rotate");
	targetOffset_ = globalVariables->GetValue<Vector3>(groupName_, "offset");
}