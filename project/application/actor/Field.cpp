#include "Field.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "ImGuiManager.h"

//コンストラクタ
Field::Field() {
}

//デストラクタ
Field::~Field() {
}

//初期化
void Field::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	//オブジェクト3dの共通部分の記録
	object3dCommon_ = object3dCommon;
	//カメラの記録
	camera_ = camera;

	//壁の初期化
	CreateWall();

	//地面
	CreateGround();
}

//更新
void Field::Update() {
	//壁
	UpdateWall();

	//地面
	UpdateGround();
}

//描画
void Field::Draw() {
	//壁
	DrawWall();

	//地面
	DrawGround();
}

//デバッグ
void Field::Debug() {
#ifdef USE_IMGUI
	//for (int32_t i = 0; i < groundGroup_.objectCount; i++) {
	//	ImGui::SeparatorText(("ground " + std::to_string(i)).c_str());
	//	ImGui::PushID(i);
	//	ImGuiManager::DragTransform(groundGroup_.fieldDescs[i].gameObject.transformData);
	//	ImGui::DragFloat3("hitBox.scale", &groundGroup_.fieldDescs[i].hitBoxScale.x, 0.1f);
	//	ImGui::PopID();
	//}

	for (int32_t i = 0; i < wallGroup_.objectCount; i++) {
		ImGui::SeparatorText(("wall " + std::to_string(i)).c_str());
		ImGui::PushID(i);
		ImGuiManager::DragTransform(wallGroup_.entity[i].gameObject.transformData);
		ImGui::DragFloat3("hitBox.scale", &wallGroup_.entity[i].hitBoxScale.x, 0.1f);
		ImGui::PopID();
	}
#endif // USE_IMGUI
}

//衝突したとき
void Field::OnCollision(ColliderState* colliderState) {
	(void)colliderState;
}

//カメラのセッター
void Field::SetCamera(Camera* camera) {
	//壁
	wallGroup_.renderObject.object3d->SetCamera(camera);
	wallGroup_.renderObject.hitBox->SetCamera(camera);

	//地面
	groundGroup_.renderObject.object3d->SetCamera(camera);
	groundGroup_.renderObject.hitBox->SetCamera(camera);
}

//地面に必要な情報のゲッター
std::vector<Entity>& Field::GetGroundDesc() {
	return groundGroup_.entity;
}

//壁に必要な情報のゲッター
std::vector<Entity>& Field::GetWallDescs() {
	return wallGroup_.entity;
}

//壁の生成
void Field::CreateWall() {
	//壁の数
	wallGroup_.objectCount = 12;
	wallGroup_.entity.resize(wallGroup_.objectCount);
	wallGroup_.modelName = "wall";
	//レンダーオブジェクトの初期化
	wallGroup_.renderObject = wallGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	wallGroup_.renderObject.object3d->Initialize(object3dCommon_, camera_, wallGroup_.objectCount);
	wallGroup_.renderObject.object3d->SetModel(wallGroup_.modelName);
	wallGroup_.renderObject.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube, wallGroup_.objectCount);

	//FieldDescの初期化
	for (int32_t i = 0; i < wallGroup_.objectCount; i++) {
		//ヒットボックスの大きさを設定
		wallGroup_.entity[i].hitBoxScale = Vector3::MakeAllOne();
		//ゲームオブジェクトの初期化
		wallGroup_.entity[i].gameObject.Initialize();

		//コライダーの状態の初期化
		wallGroup_.entity[i].colliderState.Initialize(wallGroup_.entity[i].hitBoxScale, wallGroup_.entity[i].gameObject, Tag::kWall);

		//コライダーの初期化
		wallGroup_.entity[i].collider = wallGroup_.entity[i].collider
			.SetOwner(&wallGroup_.entity[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(true)
			.SetBodyType(BodyType::kStatic)
			.SetLayer(Layer::kWall)
			.SetMaskLayer(ToBits(Layer::kWall) | ToBits(Layer::kGround) | ToBits(Layer::kEnemy) | ToBits(Layer::kPlayer))
			.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
			.Build();
	}

	wallGroup_.entity[0].gameObject.transformData = { {20.0f,4.0f,3.5f},{},{0.0f,3.0f,25.0f} };
	wallGroup_.entity[0].hitBoxScale = { 20.0f,4.0f,3.5f };

	wallGroup_.entity[1].gameObject.transformData = { {20.0f,3.0f,5.0f},{},{0.0f,2.0f,16.0f} };
	wallGroup_.entity[1].hitBoxScale = { 20.0f,3.0f,5.0f };

	wallGroup_.entity[2].gameObject.transformData = { {1.0f,1.0f,1.0f},{},{0.0f,3.0f,-16.0f} };
	wallGroup_.entity[2].hitBoxScale = { 1.0f,1.0f,1.0f };

	wallGroup_.entity[3].gameObject.transformData = { {1.0f,1.0f,1.0f},{},{0.0f,3.0f,-14.0f} };
	wallGroup_.entity[3].hitBoxScale = { 1.0f,1.0f,1.0f };

	wallGroup_.entity[4].gameObject.transformData = { {3.0f,0.2f,5.0f},{-0.3f,-5.5f,0.0f},{6.0f,6.0f,0.0f} };
	wallGroup_.entity[4].hitBoxScale = { 3.0f,0.2f,5.0f };

	wallGroup_.entity[5].gameObject.transformData = { {5.0f,0.5f,1.0f},{-0.8f,-0.7f,0.5f},{8.9f,4.4f,9.8f} };
	wallGroup_.entity[5].hitBoxScale = { 5.0f,0.5f,1.0f };

	wallGroup_.entity[6].gameObject.transformData = { {5.0f,1.0f,1.0f},{0.0f,-0.6f,0.0f},{0.6f,1.0f,-5.7f} };
	wallGroup_.entity[6].hitBoxScale = { 5.0f,1.0f,1.0f };

	wallGroup_.entity[7].gameObject.transformData = { {1.0f,3.5f,1.0f},{0.0f,0.0f,0.0f},{0.0f,4.9f,-12.0f} };
	wallGroup_.entity[7].hitBoxScale = { 1.0f,3.5f,1.0f };

	wallGroup_.entity[8].gameObject.transformData = { {20.0f,5.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,4.0f,63.0f} };
	wallGroup_.entity[8].hitBoxScale = { 20.0f,5.0f,1.0f };

	wallGroup_.entity[9].gameObject.transformData = { {20.0f,0.5f,10.0f},{0.3f,0.0f,0.0f},{0.0f,11.0f,61.0f} };
	wallGroup_.entity[9].hitBoxScale = { 20.0f,0.5f,10.0f };

	wallGroup_.entity[10].gameObject.transformData = { { 1.0f,1.0f,1.0f },{0.0f,0.0f,0.0f},{-7.8f,8.4f,47.5f} };
	wallGroup_.entity[10].hitBoxScale = { 1.0f,1.0f,1.0f };

	wallGroup_.entity[11].gameObject.transformData = { { 10.0f,0.1f,5.0f },{0.0f,1.5f,0.6f},{0.0f,5.0f,45.5f} };
	wallGroup_.entity[11].hitBoxScale = { 10.0f,0.1f,5.0f };
}

//壁の更新
void Field::UpdateWall() {
	for (int32_t i = 0; i < wallGroup_.objectCount; i++) {
		wallGroup_.renderObject.object3d->SetTransformData(i, wallGroup_.entity[i].gameObject.transformData);
		wallGroup_.renderObject.hitBox->SetScale(i, wallGroup_.entity[i].hitBoxScale);
		wallGroup_.renderObject.hitBox->SetRotate(i, wallGroup_.entity[i].gameObject.transformData.rotate);
		wallGroup_.renderObject.hitBox->SetTranslate(i, wallGroup_.renderObject.object3d->GetWorldPos(i));
	}
	wallGroup_.renderObject.object3d->Update();
	wallGroup_.renderObject.hitBox->Update();
}

//壁の描画
void Field::DrawWall() {
	wallGroup_.renderObject.object3d->Draw();
	wallGroup_.renderObject.hitBox->Draw();
}

//地面の生成
void Field::CreateGround() {
	//モデル名
	groundGroup_.modelName = "ground";

	//壁の数
	groundGroup_.objectCount = 7;
	groundGroup_.entity.resize(groundGroup_.objectCount);

	//レンダーオブジェクトの初期化
	groundGroup_
		.renderObject = groundGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	groundGroup_.renderObject.object3d->Initialize(object3dCommon_, camera_, groundGroup_.objectCount);
	groundGroup_.renderObject.object3d->SetModel(groundGroup_.modelName);
	groundGroup_.renderObject.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube, groundGroup_.objectCount);

	//FieldDescの初期化
	for (int32_t i = 0; i < groundGroup_.objectCount; i++) {
		//ヒットボックスの大きさを設定
		groundGroup_.entity[i].hitBoxScale = Vector3::MakeAllOne();
		//ゲームオブジェクトの初期化
		groundGroup_.entity[i].gameObject.Initialize();

		//コライダーの状態の初期化
		groundGroup_.entity[i].colliderState.Initialize(groundGroup_.entity[i].hitBoxScale, groundGroup_.entity[i].gameObject, Tag::kGround);

		if (i == 6) {
			groundGroup_.entity[i].colliderState.tag = Tag::kGoal;
		}

		//コライダーの初期化
		groundGroup_.entity[i].collider = groundGroup_.entity[i].collider
			.SetOwner(&groundGroup_.entity[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(true)
			.SetBodyType(BodyType::kStatic)
			.SetLayer(Layer::kGround)
			.SetMaskLayer(ToBits(Layer::kWall) | ToBits(Layer::kGround) | ToBits(Layer::kEnemy) | ToBits(Layer::kPlayer))
			.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
			.Build();
	}

	groundGroup_.entity[0].gameObject.transformData = { {20.0f,1.0f,30.0f},{},{0.0f,-2.0f,0.0f} };
	groundGroup_.entity[0].hitBoxScale = { 20.0f,1.0f,30.0f };

	groundGroup_.entity[1].gameObject.transformData = { {20.0f,1.0f,20.0f},{},{0.0f,-2.0f,55.0f} };
	groundGroup_.entity[1].hitBoxScale = { 20.0f,1.0f,20.0f };

	groundGroup_.entity[2].gameObject.transformData = { {5.0f,1.0f,5.0f},{12.0f,0.0f,0.0f},{-8.5f,0.0f,85.0f} };
	groundGroup_.entity[2].hitBoxScale = { 5.0f,1.0f,5.0f };

	groundGroup_.entity[3].gameObject.transformData = { {7.0f,1.0f,5.0f},{-0.3f,0.8f,0.0f},{10.0f,-2.0f,85.0f} };
	groundGroup_.entity[3].hitBoxScale = { 7.0f,1.0f,5.0f };

	groundGroup_.entity[4].gameObject.transformData = { { 8.0f,1.0f,6.0f},{0.1f,0.0f,0.0f},{0.0f,-2.0f,101.0f} };
	groundGroup_.entity[4].hitBoxScale = { 8.0f,1.0f,6.0f };

	groundGroup_.entity[5].gameObject.transformData = { {10.0f,1.0f,9.0f},{0.0f,2.0f,0.0f},{15.0f,-2.0f,120.0f} };
	groundGroup_.entity[5].hitBoxScale = { 10.0f,1.0f,9.0f };

	groundGroup_.entity[6].gameObject.transformData = { {10.0f,1.0f,9.0f},{},{15.0f,-2.0f,145.0f} };
	groundGroup_.entity[6].hitBoxScale = { 10.0f,1.0f,9.0f };
}

//地面の更新
void Field::UpdateGround() {
	//地面
	for (int32_t i = 0; i < groundGroup_.objectCount; i++) {
		groundGroup_.renderObject.object3d->SetTransformData(i, groundGroup_.entity[i].gameObject.transformData);
		groundGroup_.renderObject.hitBox->SetScale(i, groundGroup_.entity[i].hitBoxScale);
		groundGroup_.renderObject.hitBox->SetRotate(i, groundGroup_.entity[i].gameObject.transformData.rotate);
		groundGroup_.renderObject.hitBox->SetTranslate(i, groundGroup_.renderObject.object3d->GetWorldPos(i));
	}
	groundGroup_.renderObject.object3d->Update();
	groundGroup_.renderObject.hitBox->Update();
}

//地面の描画
void Field::DrawGround() {
	//地面
	groundGroup_.renderObject.object3d->Draw();
	groundGroup_.renderObject.hitBox->Draw();
}
