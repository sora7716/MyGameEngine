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
}

//更新ww
void TestPlayScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	object3d_->SetTransformData(0, transformData_);
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

	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {

			ImGui::Text("%5.3f", rotateMatrix.m[i][j]);

			if (j < 3) {
				ImGui::SameLine();
			}
		}
	}

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
