#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "SceneManager.h"
#include "Text.h"
#include "Core.h"
#include "Object3d.h"
#include "algorithm/Math.h"
#include "algorithm/ColliderManager.h"
#include <string>

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
	uint32_t objectCount = 2;
	object3d_->Initialize(sceneContext_.object3dCommon, camera_, objectCount);
	object3d_->SetModel("sphere");

	gameObject_.resize(objectCount);
	//初期化
	for (GameObject& gameObject : gameObject_) {
		gameObject.Initialize();
	}
	gameObject_[0].transformData.translate = { 4.0f,0.0f,0.0f };
	gameObject_[1].transformData.translate = { 0.0f,0.0f,0.0f };

	colliderStates_.resize(objectCount);
	colliders_.resize(objectCount);
	for (uint32_t i = 0; i < objectCount; i++) {
		colliderStates_[i].Initialize(gameObject_[i], physicsData_, scale);
		colliders_[i].owner = &colliderStates_[i];
		colliders_[i].isEnabled = true;
		colliders_[i].isTrigger = false;
		colliders_[i].bodyType = BodyType::kDynamic;
	}
	colliders_[0].layer = Layer::kPlayer;
	colliders_[0].maskLayer = ColliderManager::ToBit(Layer::kEnemy);
	colliders_[1].layer = Layer::kEnemy;
	colliders_[1].maskLayer = ColliderManager::ToBit(Layer::kPlayer);

	for (uint32_t i = 0; i < objectCount; i++) {
		colliderManager_->AddCollider(&colliders_[i]);
	}
}

//更新ww
void TestPlayScene::Update() {
	//シーンのインタフェースの初期化
	IScene::Update();

	for (int32_t i = 0; i < gameObject_.size(); i++) {
		object3d_->SetGameObject(i, gameObject_[i]);
	}
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

	//gameObject_.transformData.quaternion = Quaternion::Slerp(start, end, frame_).Normalize();
	//Object3d
	ImGui::Begin("object3d");
	for (int32_t i = 0; i < gameObject_.size(); i++) {
		ImGui::PushID(i);
		ImGui::SeparatorText(("object:" + std::to_string(i)).c_str());
		ImGui::DragFloat3("translate", &gameObject_[i].transformData.translate.x, 0.1f);
		ImGui::Checkbox("isAlive", &gameObject_[i].isAlive);
		ImGui::PopID();
	}
	//ImGuiManager::DragTransform(transformData_);
	//ImGui::DragFloat4("rotate", &transformData_.quaternion.x, 0.1f);
	//ImGui::DragFloat3("axis", &axis_.x, 0.1f);

	//if (ImGui::DragFloat3("eulerAngle", &eulerAngle_.x, 0.1f)) {
	//	transformData_.quaternion = Quaternion::MakeQuaternionForEulerAngle(eulerAngle_);
	//} else if (ImGui::DragFloat("angle", &angle_, 0.1f)) {
	//	transformData_.quaternion = Rendering::MakeRotateAxisAngleQuaternion(axis_, angle_).Normalize();
	//}
	//if (ImGui::Button("isAnimation")) {
	//	isAnimation_ = true;
	//}

	//if (ImGui::Button("Reset")) {
	//	frame_ = 0.0f;
	//	isAnimation_ = false;
	//}
	//ImGui::DragFloat("frame", &frame_);
	//ImGui::DragFloat4("start", &start.x, 0.1f);
	//ImGui::DragFloat4("end", &end.x, 0.1f);
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
