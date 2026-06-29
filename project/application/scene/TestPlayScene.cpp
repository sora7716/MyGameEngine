#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "ImGuiManager.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Model.h"
#include "Mesh.h"
#include "Cube.h"
#include "Frustum.h"
#include "Line.h"
#include "Plane.h"
#include "Sphere.h"
#include "algorithms/Collision.h"

//コンストラクタ
TestPlayScene::TestPlayScene() {};

//デストラクタ
TestPlayScene::~TestPlayScene() {};

//初期化
void TestPlayScene::Initialize(const SceneContext& sceneContext) {
	//ベースシーンの初期化
	BaseScene::Initialize(sceneContext);
	renderCamera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");
	gameCamera_ = sceneContext_.cameraManager->FindCamera("testPlayCamera");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneContext_.object3dCommon, &renderCamera_, 1);
	object3d_->SetGameCamera(gameCamera_);
	//object3d_->SetModel("sphere");
	object3d_->SetModel("sphere");
	//for (uint32_t i = 0; i < 33; i++) {
	//	object3d_->SetTexture(i, "magenta1x1.png");
	//}
	//object3d_->SetModel("cube");
	object3d_->SetLODDistances({ 20.0f,50.0f,80.0f });

	std::unique_ptr<GameObject>tree = std::make_unique<GameObject>();
	tree->Initialize("tree");
	tree->GetTransform().translate = { 0.0f,0.0f,10.0f };
	tree->GetTransform().scale = Vector3::MakeAllOne();

	GameObject* treePtr = tree.get();

	gameObjects_.push_back(std::move(tree));
	object3d_->AddInstance(treePtr);

	//for (uint32_t i = 0; i < object3d_->GetModel()->GetMeshes().size(); i++) {
	//	transform2ds_.push_back({ object3d_->GetUVScale(i),object3d_->GetUVRotate(i),object3d_->GetUVTranslate(i) });
	//}

	frustum_ = std::make_unique<Primitive::Frustum>();
	frustum_->Initialize(sceneContext_.directXBase, &renderCamera_);
	frustum_->SetTargetCamera(gameCamera_);

	cube_ = std::make_unique<Primitive::Cube>();
	cube_->Initialize(sceneContext_.directXBase, &renderCamera_);
}

//更新
void TestPlayScene::Update() {
	//ベースシーンの更新
	BaseScene::Update();

	//for (uint32_t i = 0; i < object3d_->GetModel()->GetMeshes().size(); i++) {
	//	object3d_->SetUVScale(i, transform2ds_[i].scale);
	//	object3d_->SetUVRotate(i, transform2ds_[i].rotate);
	//	object3d_->SetUVTranslate(i, transform2ds_[i].translate);
	//}

	object3d_->Update();

	frustum_->Update();

	cube_->Update();

	if (Collision::IsCollision(gameCamera_->GetFrustum(), cube_->GetAABB())) {
		cube_->SetColor(Vector4::MakeRedColor());
	} else {
		cube_->SetColor(Vector4::MakeWhiteColor());
	}

	if (debugCamera_->IsDebug()) {
		renderCamera_ = *debugCamera_->GetCamera();
	} else {
		renderCamera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");
	}

	if (sceneContext_.input->PressKey(DIK_UP)) {
		gameObjects_[0]->GetTransform().translate.z -= 1.0f;
	} else if (sceneContext_.input->PressKey(DIK_DOWN)) {
		gameObjects_[0]->GetTransform().translate.z += 1.0f;
	}
	;
}

//デバッグ
void TestPlayScene::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("object3d");

	for (int32_t i = 0; i < gameObjects_.size(); i++) {
		ImGui::PushID(i);

		if (ImGui::TreeNode(("object" + std::to_string(i)).c_str())) {
			ImGuiManager::DragTransform(gameObjects_[i]->GetTransform());
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
	//for (uint32_t i = 0; i < transform2ds_.size(); i++) {
	//	ImGui::PushID(i);
	//	ImGui::DragFloat2("uvScale", &transform2ds_[i].scale.x, 0.1f);
	//	ImGui::DragFloat("uvRotate", &transform2ds_[i].rotate, 0.1f);
	//	ImGui::DragFloat2("uvTranslate", &transform2ds_[i].translate.x, 0.1f);
	//	ImGui::PopID();
	//}
	Vector3 cameraTranslate2 = gameCamera_->GetTranslate();
	ImGui::Text("cameraToPlayer:%f", gameObjects_[0]->GetTransform().translate - cameraTranslate2);
	ImGui::End();

	ImGui::Begin("camera");
	Vector3 cameraTranslate = gameCamera_->GetTranslate();
	Vector3 cameraRotate = gameCamera_->GetEulerAngle();
	ImGui::DragFloat3("rotate", &cameraRotate.x, 0.1f);
	ImGui::DragFloat3("translate", &cameraTranslate.x, 0.1f);
	gameCamera_->SetEulerAngle(cameraRotate);
	gameCamera_->SetTranslate(cameraTranslate);
	ImGui::End();

	ImGui::Begin("cube");
	PrimitiveData::OBB obb = cube_->GetOBB();
	ImGui::DragFloat3("size", &obb.size.x, 0.1f);
	ImGui::DragFloat3("translate", &obb.center.x, 0.1f);
	cube_->SetOBB(obb);
	ImGui::End();
#endif // USE_IMGUI

//#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		renderCamera_ = *debugCamera_->GetCamera();
	} else {
		renderCamera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");
	}
//#endif // _DEBUG
}

//描画
void TestPlayScene::Draw() {
	object3d_->Draw();

	frustum_->Draw();

	cube_->Draw();
}

//終了
void TestPlayScene::Finalize() {
	//ベースシーンのの終了
	BaseScene::Finalize();
}
