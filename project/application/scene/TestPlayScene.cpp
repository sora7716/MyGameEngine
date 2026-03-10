#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "SceneManager.h"
#include "Text.h"
#include "Core.h"
#include "Object3d.h"
#include "func/Math.h"

//コンストラクタ
TestPlayScene::TestPlayScene() {
};

//デストラクタ
TestPlayScene::~TestPlayScene() {
};

//初期化
void TestPlayScene::Initialize(const SceneContext& sceneContext) {
	//シーンのインタフェースの初期化
	IScene::Initialize(sceneContext);
	camera_ = sceneContext_.cameraManager->FindCamera("testPlayCamera");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneContext_.object3dCommon, camera_);
	object3d_->SetModel("sphere");
	transformData_.scale = Vector3::MakeAllOne();
	transformData_.quaternion = Quaternion::IdentityQuaternion();

}

//更新ww
void TestPlayScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	object3d_->SetTransformData(0, transformData_);
	object3d_->SetCamera(camera_);
	object3d_->Update();
#ifdef USE_IMGUI
	//ImGuiの受付開始
	sceneContext_.imguiManager->Begin();
	//デバッグカメラ
	ImGui::Begin("debugCamera");
	debugCamera_->Debug();
	ImGui::End();

	if (isAnimation_) {
		if (frame_ < 1.0f) {
			frame_ += Math::kDeltaTime;
		}
	}

	transformData_.quaternion = Quaternion::Slerp(start, end, frame_).Normalize();
	//Object3d
	ImGui::Begin("object3d");
	//ImGuiManager::DragTransform(transformData_);
	//ImGui::DragFloat4("rotate", &transformData_.quaternion.x, 0.1f);
	//ImGui::DragFloat3("axis", &axis_.x, 0.1f);

	//if (ImGui::DragFloat3("eulerAngle", &eulerAngle_.x, 0.1f)) {
	//	transformData_.quaternion = Quaternion::MakeQuaternionForEulerAngle(eulerAngle_);
	//} else if (ImGui::DragFloat("angle", &angle_, 0.1f)) {
	//	transformData_.quaternion = Rendering::MakeRotateAxisAngleQuaternion(axis_, angle_).Normalize();
	//}
	if (ImGui::Button("isAnimation")) {
		isAnimation_ = true;
	}

	if (ImGui::Button("Reset")) {
		frame_ = 0.0f;
		isAnimation_ = false;
	}
	ImGui::DragFloat("frame", &frame_);
	ImGui::DragFloat4("start", &start.x, 0.1f);
	ImGui::DragFloat4("end", &end.x, 0.1f);
	ImGui::End();

	//ImGuiの受付終了
	sceneContext_.imguiManager->End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = debugCamera_->GetCamera();
	} else {
		camera_ = sceneContext_.cameraManager->FindCamera("testPlayCamera");
	}
#endif // _DEBUG
}

//描画
void TestPlayScene::Draw() {
	object3d_->Draw();
}

//終了
void TestPlayScene::Finalize() {
	//シーンのインターフェースの終了
	IScene::Finalize();
}
