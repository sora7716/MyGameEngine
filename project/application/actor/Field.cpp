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

	//モデル名
	ground_.modelName = "ground";

	//壁の数
	ground_.objectCount = 7;
	ground_.fieldDescs.resize(ground_.objectCount);

	//レンダーオブジェクトの初期化
	ground_.renderObject = ground_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	ground_.renderObject.object3d->Initialize(object3dCommon_, camera_, ground_.objectCount);
	ground_.renderObject.object3d->SetModel(ground_.modelName);
	ground_.renderObject.hitBox->Initialize(object3dCommon_->GetWireframeObject3dCommon(), camera_, ModelType::kCube, ground_.objectCount);

	//FieldDescの初期化
	for (int32_t i = 0; i < ground_.objectCount; i++) {
		//ヒットボックスの大きさを設定
		ground_.fieldDescs[i].hitBoxScale = Vector3::MakeAllOne();
		//ゲームオブジェクトの初期化
		ground_.fieldDescs[i].gameObject.Initialize();

		//コライダーの状態の初期化
		ground_.fieldDescs[i].colliderState.Initialize(ground_.fieldDescs[i].hitBoxScale, ground_.fieldDescs[i].gameObject, ground_.renderObject.object3d->GetWorldMatrix(i), Tag::kGround);

		if (i == 6) {
			ground_.fieldDescs[i].colliderState.tag = Tag::kGoal;
		}

		//コライダーの初期化
		ground_.fieldDescs[i].collider = ground_.fieldDescs[i].collider
			.SetOwner(&ground_.fieldDescs[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(true)
			.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
			.Build();
	}

	ground_.fieldDescs[0].gameObject.transformData = { {20.0f,1.0f,30.0f},{},{0.0f,-2.0f,0.0f} };
	ground_.fieldDescs[0].hitBoxScale = { 20.0f,1.0f,30.0f };

	ground_.fieldDescs[1].gameObject.transformData = { {20.0f,1.0f,20.0f},{},{0.0f,-2.0f,55.0f} };
	ground_.fieldDescs[1].hitBoxScale = { 20.0f,1.0f,20.0f };

	ground_.fieldDescs[2].gameObject.transformData = { {5.0f,1.0f,5.0f},{12.0f,0.0f,0.0f},{-8.5f,0.0f,85.0f} };
	ground_.fieldDescs[2].hitBoxScale = { 5.0f,1.0f,5.0f };

	ground_.fieldDescs[3].gameObject.transformData = { {7.0f,1.0f,5.0f},{-0.3f,0.8f,0.0f},{10.0f,-2.0f,85.0f} };
	ground_.fieldDescs[3].hitBoxScale = { 7.0f,1.0f,5.0f };

	ground_.fieldDescs[4].gameObject.transformData = { { 8.0f,1.0f,6.0f},{0.1f,0.0f,0.0f},{0.0f,-2.0f,101.0f} };
	ground_.fieldDescs[4].hitBoxScale = { 8.0f,1.0f,6.0f };

	ground_.fieldDescs[5].gameObject.transformData = { {10.0f,1.0f,9.0f},{0.0f,2.0f,0.0f},{15.0f,-2.0f,120.0f} };
	ground_.fieldDescs[5].hitBoxScale = { 10.0f,1.0f,9.0f };

	ground_.fieldDescs[6].gameObject.transformData = { {10.0f,1.0f,9.0f},{},{15.0f,-2.0f,145.0f} };
	ground_.fieldDescs[6].hitBoxScale = { 10.0f,1.0f,9.0f };

}

//更新
void Field::Update() {
	//壁
	UpdateWall();

	//地面
	for (int32_t i = 0; i < ground_.objectCount; i++) {
		ground_.renderObject.object3d->SetTransformData(i, ground_.fieldDescs[i].gameObject.transformData);
		ground_.renderObject.hitBox->SetScale(i, ground_.fieldDescs[i].hitBoxScale);
		ground_.renderObject.hitBox->SetRotate(i, ground_.fieldDescs[i].gameObject.transformData.rotate);
		ground_.renderObject.hitBox->SetTranslate(i, ground_.renderObject.object3d->GetWorldPos(i));
	}
	ground_.renderObject.object3d->Update();
	ground_.renderObject.hitBox->Update();
}

//描画
void Field::Draw() {
	//壁
	//DrawWall();

	//地面
	ground_.renderObject.object3d->Draw();
	ground_.renderObject.hitBox->Draw();
}

//デバッグ
void Field::Debug() {
	for (int32_t i = 0; i < ground_.objectCount; i++) {
		ImGui::SeparatorText(("ground " + std::to_string(i)).c_str());
		ImGui::PushID(i);
		ImGuiManager::DragTransform(ground_.fieldDescs[i].gameObject.transformData);
		ImGui::DragFloat3("hitBox.scale", &ground_.fieldDescs[i].hitBoxScale.x, 0.1f);
		ImGui::PopID();
	}
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
	ground_.renderObject.object3d->SetCamera(camera);
	ground_.renderObject.hitBox->SetCamera(camera);
}

//地面に必要な情報のゲッター
std::vector<FieldObjectDesc>& Field::GetGroundDesc() {
	return ground_.fieldDescs;
}

//壁に必要な情報のゲッター
std::vector<FieldObjectDesc>& Field::GetWallDescs() {
	return wallGroup_.fieldDescs;
}

//壁の生成
void Field::CreateWall() {
	//壁の数
	wallGroup_.objectCount = 1;
	wallGroup_.fieldDescs.resize(wallGroup_.objectCount);
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
		wallGroup_.fieldDescs[i].hitBoxScale = Vector3::MakeAllOne();
		//ゲームオブジェクトの初期化
		wallGroup_.fieldDescs[i].gameObject.Initialize();

		//コライダーの状態の初期化
		wallGroup_.fieldDescs[i].colliderState.Initialize(wallGroup_.fieldDescs[i].hitBoxScale, wallGroup_.fieldDescs[i].gameObject, wallGroup_.renderObject.object3d->GetWorldMatrix(i), Tag::kWall);

		//コライダーの初期化
		wallGroup_.fieldDescs[i].collider = wallGroup_.fieldDescs[i].collider
			.SetOwner(&wallGroup_.fieldDescs[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(true)
			.SetOnCollision([this](ColliderState* other) {this->OnCollision(other); })
			.Build();
	}

	//壁の位置を指定
	wallGroup_.fieldDescs[0].gameObject.transformData.translate = { 20.0f,0.0f,25.0f };
	//wallGroup_.wallDescs[1].gameObject.transformData.translate = { 18.0f,0.0f,25.0f };
}

//壁の更新
void Field::UpdateWall() {
	for (int32_t i = 0; i < wallGroup_.objectCount; i++) {
		wallGroup_.renderObject.object3d->SetTransformData(i, wallGroup_.fieldDescs[i].gameObject.transformData);
		wallGroup_.renderObject.hitBox->SetScale(i, wallGroup_.fieldDescs[i].hitBoxScale);
		wallGroup_.renderObject.hitBox->SetRotate(i, wallGroup_.fieldDescs[i].gameObject.transformData.rotate);
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
