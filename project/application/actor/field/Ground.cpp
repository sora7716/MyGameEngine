#include "Ground.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"

//コンストラクタ
Ground::Ground() {
}

//デストラクタ
Ground::~Ground() {
}

//初期化
void Ground::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	entityGroup_.objectCount = 1;
	entityGroup_.modelName = "ground";

	entityGroup_.entity.resize(entityGroup_.objectCount);

	entityGroup_.renderObject = entityGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();

	entityGroup_.renderObject.object3d->Initialize(object3dCommon, camera, entityGroup_.objectCount);
	entityGroup_.renderObject.object3d->SetModel(entityGroup_.modelName);
	entityGroup_.renderObject.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube, entityGroup_.objectCount);

	//初期化
	for (Entity& entity : entityGroup_.entity) {
		entity.gameObject.Initialize();
		entity.colliderState.Initialize(entity.gameObject, entity.physicsData, entity.gameObject.transformData.scale);
		entity.collider.owner = &entity.colliderState;
		entity.collider.isEnabled = true;
		entity.collider.isTrigger = false;
		entity.collider.bodyType = BodyType::kStatic;
		entity.collider.layer = Layer::kGround;
		entity.collider.maskLayer = static_cast<uint32_t>(Layer::kPlayer);
	}

	entityGroup_.entity[0].gameObject.transformData.scale = { 5.0f,1.0f,5.0f };
	entityGroup_.entity[0].gameObject.transformData.translate.y = -2.0f;
}

//更新
void Ground::Update() {
	//ゲームオブジェクトなどの設定
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.renderObject.object3d->SetGameObject(i, entityGroup_.entity[i].gameObject);
		entityGroup_.renderObject.hitBox->SetTransformData(i, entityGroup_.entity[i].gameObject.transformData);
	}

	//描画オブジェクトの更新
	entityGroup_.renderObject.object3d->Update();
	entityGroup_.renderObject.hitBox->Update();
}

//デバッグ
void Ground::Debug() {
	ImGuiManager::TreeNodeForEntityGroup("ground", entityGroup_);
}

//描画
void Ground::Draw() {
	entityGroup_.renderObject.object3d->Draw();
	entityGroup_.renderObject.hitBox->Draw();
}

//カメラのセッター
void Ground::SetCamera(Camera* camera) {
	entityGroup_.renderObject.object3d->SetCamera(camera);
	entityGroup_.renderObject.hitBox->SetCamera(camera);
}

//エンティティのゲッター
std::vector<Entity>& Ground::GetEntity() {
	// TODO: return ステートメントをここに挿入します
	return entityGroup_.entity;
}
