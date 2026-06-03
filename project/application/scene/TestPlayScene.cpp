#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "ImGuiManager.h"
#include "Box.h"
#include "Object3d.h"
#include "Model.h"
#include "Mesh.h"
#include "Cube.h"
#include "Frustum.h"
#include "Line.h"
#include "Plane.h"
#include "Sphere.h"

//コンストラクタ
TestPlayScene::TestPlayScene() {};

//デストラクタ
TestPlayScene::~TestPlayScene() {};

//初期化
void TestPlayScene::Initialize(const SceneContext& sceneContext) {
	//ベースシーンの初期化
	BaseScene::Initialize(sceneContext);
	camera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");

	//box_ = std::make_unique<Box>();
	//box_->Initialize(sceneContext_.directXBase, sceneContext_.textureManager, &camera_);
	//box_->SetModel("cube");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize(sceneContext_.object3dCommon, &camera_);
	object3d_->SetModel("multiMaterial");

	for (uint32_t i = 0; i < object3d_->GetModel()->GetMeshes().size(); i++) {
		transform2ds_.push_back({ object3d_->GetUVScale(i),object3d_->GetUVRotate(i),object3d_->GetUVTranslate(i) });
	}

	frustum_ = std::make_unique<Primitive::Frustum>();
	frustum_->Initialize(sceneContext_.directXBase, &camera_);
	testPlayCamera = sceneContext_.cameraManager->FindCamera("testPlayCamera");
	frustum_->SetTargetCamera(testPlayCamera);

	plane_ = std::make_unique<Primitive::Plane>();
	plane_->Initialize(sceneContext_.directXBase, &camera_);
	plane_->SetColor(Vector4::MakeRedColor());

	sphere_ = std::make_unique<Primitive::Sphere>();
	sphere_->Initialize(sceneContext_.directXBase,&camera_);
}

//更新
void TestPlayScene::Update() {
	//ベースシーンの更新
	BaseScene::Update();

	for (uint32_t i = 0; i < object3d_->GetModel()->GetMeshes().size(); i++) {
		object3d_->SetUVScale(i, transform2ds_[i].scale);
		object3d_->SetUVRotate(i, transform2ds_[i].rotate);
		object3d_->SetUVTranslate(i, transform2ds_[i].translate);
	}

	object3d_->Update();

	frustum_->Update();

	plane_->Update();

	sphere_->Update();

	//box_->Update();
}

//デバッグ
void TestPlayScene::Debug() {
#ifdef USE_IMGUI
	//ImGui::Begin("box");
	//box_->Debug();
	//ImGui::End();

	ImGui::Begin("object3d");
	GameObject gameObject = object3d_->GetGameObject(0);
	ImGuiManager::DebugGameObject(gameObject);
	object3d_->SetGameObject(0, gameObject);
	for (uint32_t i = 0; i < transform2ds_.size(); i++) {
		ImGui::PushID(i);
		ImGui::DragFloat2("uvScale", &transform2ds_[i].scale.x, 0.1f);
		ImGui::DragFloat("uvRotate", &transform2ds_[i].rotate, 0.1f);
		ImGui::DragFloat2("uvTranslate", &transform2ds_[i].translate.x, 0.1f);
		ImGui::PopID();
	}
	ImGui::End();

	ImGui::Begin("camera");
	Vector3 cameraTranslate = testPlayCamera->GetTranslate();
	Vector3 cameraRotate = testPlayCamera->GetEulerAngle();
	ImGui::DragFloat3("rotate", &cameraRotate.x, 0.1f);
	ImGui::DragFloat3("translate", &cameraTranslate.x, 0.1f);
	testPlayCamera->SetEulerAngle(cameraRotate);
	testPlayCamera->SetTranslate(cameraTranslate);
	ImGui::End();

	ImGui::Begin("plane");
	PrimitiveData::Plane planeData = plane_->GetPlane();
	ImGui::DragFloat3("normal", &planeData.normal.x, 0.1f);
	ImGui::DragFloat("distance", &planeData.distance, 0.1f);
	plane_->SetPlane(planeData);
	ImGui::End();

	ImGui::Begin("sphere");
	PrimitiveData::Sphere sphereData = sphere_->GetSphere();
	ImGui::DragFloat3("center", &sphereData.center.x, 0.1f);
	ImGui::DragFloat("radius", &sphereData.radius, 0.1f);
	sphere_->SetSphere(sphereData);
	ImGui::End();
#endif // USE_IMGUI

#ifdef _DEBUG
	if (debugCamera_->IsDebug()) {
		camera_ = *debugCamera_->GetCamera();
	} else {
		camera_ = *sceneContext_.cameraManager->FindCamera("testPlayCamera");
	}
#endif // _DEBUG
}

//描画
void TestPlayScene::Draw() {
	//box_->Draw();

	//object3d_->Draw();

	//frustum_->Draw();

	plane_->Draw();

	sphere_->Draw();
}

//終了
void TestPlayScene::Finalize() {
	//ベースシーンのの終了
	BaseScene::Finalize();
}
