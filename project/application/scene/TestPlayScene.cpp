#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "SceneManager.h"
#include "Text.h"
#include "Core.h"
#include "Object3d.h"

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

	//Object3d
	ImGui::Begin("object3d");
	ImGuiManager::DragTransform(transformData_);
	ImGui::End();

	ImGui::Begin("MT4_01_00");
	ImGuiManager::Matrix4x4Text(rotateMatrix, "rotateAxisAngle");
	ImGui::End();

	ImGui::Begin("MT4_01_03");
	ImGuiManager::QuaternionText(identity, "Identity");
	ImGuiManager::QuaternionText(conj, "Conjugate");
	ImGuiManager::QuaternionText(inv, "Inverse");
	ImGuiManager::QuaternionText(normal, "Normalize");
	ImGuiManager::QuaternionText(mul1, "Multiply(q1,q2)");
	ImGuiManager::QuaternionText(mul2, "Multiply(q2,q1)");
	ImGuiManager::FloatText(norm, "Norm");
	ImGui::End();

	ImGui::Begin("MT4_01_04");
	ImGuiManager::QuaternionText(rotation, "rotation");
	ImGuiManager::Matrix4x4Text(rotateMat, "rotateMatrix");
	ImGuiManager::Vector3Text(rotateByQuaternion, "rotateByQuaternion");
	ImGuiManager::Vector3Text(rotateByMatrix, "rotateByMatrix");
	ImGui::End();

	ImGui::Begin("MT4_01_05");
	ImGuiManager::QuaternionText(interpolate0, "interpolate0, Slerp(q0, q1, 0.0f)");
	ImGuiManager::QuaternionText(interpolate1, "interpolate0, Slerp(q0, q1, 0.3f)");
	ImGuiManager::QuaternionText(interpolate2, "interpolate0, Slerp(q0, q1, 0.5f)");
	ImGuiManager::QuaternionText(interpolate3, "interpolate0, Slerp(q0, q1, 0.7f)");
	ImGuiManager::QuaternionText(interpolate4, "interpolate0, Slerp(q0, q1, 1.0f)");
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
