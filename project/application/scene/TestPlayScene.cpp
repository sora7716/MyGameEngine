#include "TestPlayScene.h"
#include "Input.h"
#include "CameraManager.h"
#include "ImGuiManager.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Model.h"
#include "Mesh.h"
#include "Collision.h"
#include "SkyBox.h"
#include "Sprite.h"
#include "Cube.h"
#include "Circle.h"
#include "Plane.h"
#include "Line.h"
#include "Sphere.h"
#include "Frustum.h"
#include "ParticleSystem.h"
#include "ModelManager.h"
#include "LightingManager.h"
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

	directionalLight_ = *sceneContext_.lightingManager->GetDirectionalLight();

	GameObject* gameObject = CreateGameObject();
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	sceneContext_.modelManager->FindModel("dekanu")->CreateLODModels({ 1.0f,0.75f,0.5f,0.25f });
	object3d->SetModel(sceneContext_.modelManager->FindModel("dekanu"));
	object3d->SetLODDistances({ 20.0f,40.0f,60.0f });
	gameObject->SetName("デカヌチャン");

	GameObject* spriteObject = CreateGameObject();
	Sprite* sprite = spriteObject->AddComponent<Sprite>();
	sprite->ChangeTexture("uvChecker.png");
	spriteObject->SetName("uvChecker");

	GameObject* spriteObject2 = CreateGameObject();
	Sprite* sprite2 = spriteObject2->AddComponent<Sprite>();
	sprite2->ChangeTexture("monsterBall.png");
	spriteObject2->SetName("モンスターボール");

	GameObject* skyBoxGameObject = CreateGameObject();
	skyBoxGameObject->AddComponent<SkyBox>();
	skyBoxGameObject->SetName("skyBox");
	skyBoxGameObject->GetTransform().scale = { 50.0f,50.0f,50.0f };

	GameObject* cubeWireframe = CreateGameObject();
	cubeWireframe->AddComponent<debugDraw::Plane>();
	cubeWireframe->SetName("ワイヤーフレーム");

	GameObject* frustumObject = CreateGameObject();
	debugDraw::Frustum* frustum = frustumObject->AddComponent<debugDraw::Frustum>();
	frustum->SetTargetCamera(gameCamera_);
	frustumObject->SetName("カメラの視錐台");

	GameObject* particleObject = CreateGameObject();
	ParticleSystem* particleSystem = particleObject->AddComponent<ParticleSystem>();
	particleSystem->SetModel(sceneContext_.modelManager->FindModel("plane"));
	particleSystem->SetTexture(0, "circle2.png");
	particleObject->SetName("particleSystem");
	particleSystem->SetFrequency(0.5f);
}

//更新
void TestPlayScene::Update(){
	//ベースシーンの更新
	BaseScene::Update();
}

//デバッグ
void TestPlayScene::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("Object");
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

	if (ImGui::TreeNode("directionalLight")){
		directionalLight_ = *sceneContext_.lightingManager->GetDirectionalLight();
		ImGui::ColorEdit4("color", &directionalLight_.color.x);
		ImGui::DragFloat3("direction", &directionalLight_.direction.x, 0.01f);
		ImGui::DragFloat("intensity", &directionalLight_.intensity, 0.05f, 0.0f, 10.0f);
		sceneContext_.lightingManager->SetDirectionalLight(directionalLight_);
		ImGui::TreePop();
	}

	ImGui::End();
#endif // USE_IMGUI
}

//終了
void TestPlayScene::Finalize(){
	//ベースシーンのの終了
	BaseScene::Finalize();
}
