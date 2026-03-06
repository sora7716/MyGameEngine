#include "RailCamera.h"
#include "engine/camera/Camera.h"
#include "engine/debug/ImGuiManager.h"
#include "engine/worldTransform/WorldTransform.h"
#include "engine/3d/Object3d.h"
#include "engine/math/func/Math.h"
#include "engine/input/Input.h"

//初期化
void RailCamera::Initialize(Input* input, Camera* camera, const Quaternion& rotate, const Vector3& position) {
	//入力を記録
	input_ = input;

	//カメラを記録
	camera_ = camera;

	//カメラのオブジェクト
	/*cameraObject_ = new Object3d();
	cameraObject_->Initialize(camera_);
	cameraObject_->SetModel("camera");*/

	//マテリアル
	material_.color = { 1.0f,1.0f,1.0f,1.0f };
	material_.enableLighting = true;
	material_.shininess = 10.0f;
	material_.uvMatrix = Matrix4x4::Identity4x4();

	//回転を記録
	rotate_ = rotate;
	//位置を記録
	translate_ = position;
}

//更新
void RailCamera::Update() {
	//カメラを動かすフラグのスイッチ
	if (input_->TriggerKey(DIK_RETURN) && input_->PressKey(DIK_LSHIFT)) {
		isMovingCamera_ = !isMovingCamera_;
	}

	//カメラを動かす
	if (isMovingCamera_) {
		if (frame_ < endFrame_ - 1.0f) {
			frame_++;
		} else {
			frame_ = 0.0f;
		}
	}

	//カメラの動き
	//translate_ = Math::CatmullRomPosition(controlPoints_, frame_ / endFrame_);

	//カメラの設定
	camera_->SetQuaternion(rotate_);
	camera_->SetTranslate(translate_);
	camera_->SetFovY(fovY_);
	////カメラのオブジェクト
	//cameraObject_->SetRotate(rotate_);
	//cameraObject_->SetTranslate(translate_);
	////マテリアルの適応
	//cameraObject_->GetModel()->SetMaterial(material_);

	////カメラのオブジェクトの更新
	//cameraObject_->Update();
}

//デバッグ
void RailCamera::Debug() {
#ifdef USE_IMGUI
	ImGui::DragFloat3("rotate", &rotate_.x, 0.1f);
	ImGui::DragFloat3("translate", &translate_.x, 0.1f);
	ImGui::DragFloat("FovY", &fovY_, 0.1f);
#endif // USE_IMGUI
}

//描画
void RailCamera::Draw() {
	//cameraObject_->Draw();
}

//終了
void RailCamera::Finalize() {
	//delete cameraObject_;
	//cameraObject_ = nullptr;
}

//ワールドトランスフォームのゲッター
const WorldTransform* RailCamera::GetWorldTransform() const {
	//return cameraObject_->GetWorldTransform();
	return {};
}

//カメラのゲッター
Camera* RailCamera::GetCamera() {
	return camera_;
}

//カメラのセッター
void RailCamera::SetCamera(Camera* camera) {
	//cameraObject_->SetCamera(camera);
	(void)camera;
}

//制御ポイントのセッター
void RailCamera::SetControlPoints(std::vector<Vector3> controlPoints) {
	controlPoints_.resize(controlPoints_.size());
	controlPoints_ = controlPoints;
}
