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
#include "Collision.h"
#include "TextureManager.h"
#include "SkyBox.h"
#include "ParticleSystem.h"
#include "ModelManager.h"
#include "WinApi.h"
#include "LightingManager.h"
#include "Object3dRenderer.h"
#include "SkyBoxRenderer.h"
#include "DebugDrawRenderer.h"
#include <numbers>

//コンストラクタ
TestPlayScene::TestPlayScene(){};

//デストラクタ
TestPlayScene::~TestPlayScene(){};

//初期化
void TestPlayScene::Initialize(){
	//ベースシーンの初期化
	BaseScene::Initialize();
	gameCamera_ = sceneContext_.cameraManager->FindCamera("testPlayCamera");

	object3d_ = Object3d::Create(sceneContext_.directXBase, gameCamera_);
	object3d_->SetGameCamera(gameCamera_);
	object3d_->SetModel(sceneContext_.modelManager->FindModel("cube"), { 1.0f });
	//object3d_->SetTexture(0, "uvChecker.png");
	//object3d_->SetModel("cube");
	//object3d_->SetModel("dekanu");
	//for (uint32_t i = 0; i < 33; i++) {
	//	object3d_->SetTexture(i, "white1x1.png");
	//}
	object3d_->RegisterToRenderer(object3dRenderer_);

	//object3d_->SetLODDistances({ 20.0f,30.0f,50.0f,80.0f });
	GameObject* treePtr = nullptr;
	std::unique_ptr<GameObject>modelObject = std::make_unique<GameObject>();
	modelObject->Initialize("object3d");
	modelObject->GetTransform().translate = { 0.0f,0.0f,-9.9f };
	modelObject->GetTransform().eulerAngle = { -std::numbers::pi_v<float> / 2.0f,0.0f,0.0f };
	modelObject->GetTransform().quaternion = Quaternion::MakeQuaternionForEulerAngle(modelObject->GetTransform().eulerAngle);
	modelObject->GetTransform().scale = Vector3::MakeAllOne();

	treePtr = modelObject.get();
	gameObjects_.push_back(std::move(modelObject));
	object3d_->AddInstance(treePtr);

	std::unique_ptr<GameObject> skyBoxObject = std::make_unique<GameObject>();
	skyBoxObject->Initialize("skyBox");
	skyBoxObject->GetTransform().scale = { 50.0f,50.0f,50.0f };
	treePtr = skyBoxObject.get();
	gameObjects_.push_back(std::move(skyBoxObject));

	skyBox_ = std::make_unique<SkyBox>();
	skyBox_->Initialize(sceneContext_.directXBase, "skybox_cube.dds", gameCamera_);
	skyBox_->SetGameObject(treePtr);
	for (uint32_t i = 0; i < object3d_->GetMeshDataSize(); i++){
		object3d_->SetEnvironmentMap(i, "skybox_cube.dds");
	}
	//for (uint32_t i = 0; i < object3d_->GetModel()->GetMeshes().size(); i++) {
	//	transform2ds_.push_back({ object3d_->GetUVScale(i),object3d_->GetUVRotate(i),object3d_->GetUVTranslate(i) });
	//}

	frustum_ = std::make_unique<debugDraw::Frustum>();
	frustum_->Initialize(sceneContext_.directXBase, gameCamera_);
	frustum_->SetTargetCamera(gameCamera_);
	frustum_->SetColor({ 1.0f,0.0f,0.0f,1.0f });

	cube_ = std::make_unique<debugDraw::Cube>();
	cube_->Initialize(sceneContext_.directXBase, gameCamera_);

	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize(sceneContext_.particleCommon, gameCamera_, "circle2.png");
	particleSystem_->SetGameCamera(gameCamera_);
	particleSystem_->SetParticleCount(2);
	particleSystem_->SetFrequency(0.3f);
	//particleSystem_->SetModelData(sceneContext_.object3dCommon->GetModelManager()->FindModel("dekanu")->GetModelData());
	directionalLight_ = *sceneContext_.lightingManager->GetDirectionalLight();
}

//更新
void TestPlayScene::Update(){
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

	skyBox_->Update();

	particleSystem_->Update();

	if (collision::IsCollision(gameCamera_->GetFrustum(), cube_->GetAABB())){
		cube_->SetColor(Vector4::MakeRedColor());
	} else{
		cube_->SetColor(Vector4::MakeWhiteColor());
	}
}

//デバッグ
void TestPlayScene::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("Object");

	for (int32_t i = 0; i < gameObjects_.size(); i++){
		ImGui::PushID(i);

		if (ImGui::TreeNode(("object" + std::to_string(i)).c_str())){
			ImGuiManager::DragTransform(gameObjects_[i]->GetTransform());
			ImGui::SliderFloat("environmentCoefficient", &environmentCoefficient_, 0.0f, 1.0f);
			object3d_->SetEnvironmentCoefficient(0, environmentCoefficient_);
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

	if (ImGui::TreeNode("camera")){
		Vector3 cameraTranslate = gameCamera_->GetTranslate();
		Vector3 cameraRotate = gameCamera_->GetEulerAngle();
		ImGui::DragFloat3("rotate", &cameraRotate.x, 0.1f);
		ImGui::DragFloat3("translate", &cameraTranslate.x, 0.1f);
		gameCamera_->SetEulerAngle(cameraRotate);
		gameCamera_->SetTranslate(cameraTranslate);

		float farClip = gameCamera_->GetFarClip();
		ImGui::DragFloat("farClip", &farClip);
		gameCamera_->SetFarClip(farClip);
		ImGui::TreePop();
	}

	//if (ImGui::TreeNode("skyBox")) {
	//	ImGuiManager::DragTransform(skyBoxObject_->GetTransform());
	//	ImGui::TreePop();
	//}

	if (ImGui::TreeNode("particle")){
		ImGui::DragFloat3("emitter", &emitterPos_.x, 0.01f);
		particleSystem_->SetEmitterPosition(emitterPos_);
		primitiveData::OBB obb = cube_->GetOBB();
		obb.center = emitterPos_;
		cube_->SetOBB(obb);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("directionalLight")){
		ImGui::ColorEdit4("color", &directionalLight_.color.x);
		ImGui::DragFloat3("direction", &directionalLight_.direction.x, 0.01f);
		ImGui::DragFloat("intensity", &directionalLight_.intensity);
		sceneContext_.lightingManager->SetDirectionalLight(directionalLight_);
		ImGui::TreePop();
	}

	ImGui::End();
#endif // USE_IMGUI
}

//描画
void TestPlayScene::Draw(Camera* camera){
	object3d_->SetRenderCamera(camera);
	object3dRenderer_->AddRenderData(object3d_->GetRenderData());

	frustum_->SetRenderCamera(camera);
	debugDrawRenderer_->AddRenderData(frustum_->GetRenderData());

	cube_->SetRenderCamera(camera);
	debugDrawRenderer_->AddRenderData(cube_->GetRenderData());

	skyBox_->SetRenderCamera(camera);
	skyBoxRenderer_->AddRenderData(skyBox_->GetSkyBoxRenderData());
	//skyBox_->Draw(sceneContext_.textureManager);

	particleSystem_->SetRenderCamera(camera);
	particleSystem_->Draw();
}

//デバッグでの描画
void TestPlayScene::DebugDraw(){
	Draw(debugCamera_->GetCamera());
}

//ゲームでの描画
void TestPlayScene::GameDraw(){
	Draw(gameCamera_);
}

//終了
void TestPlayScene::Finalize(){
	//ベースシーンのの終了
	BaseScene::Finalize();
}
