#include "BaseGround.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"
#include"algorithms/Math.h"

//コンストラクタ
BaseGround::BaseGround() {
	entityGroup_.objectCount = 1;
	entityGroup_.modelName = "ground";
}

//デストラクタ
BaseGround::~BaseGround() {
}

//初期化
void BaseGround::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	entityGroup_.entity.resize(entityGroup_.objectCount);

	entityGroup_.renderObject = entityGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();

	entityGroup_.renderObject.object3d->Initialize(object3dCommon, camera, entityGroup_.objectCount);
	entityGroup_.renderObject.object3d->SetModel(entityGroup_.modelName);
	entityGroup_.renderObject.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube, entityGroup_.objectCount);

	//初期化
	for (uint32_t i = 0; i < static_cast<uint32_t>(entityGroup_.entity.size()); i++) {
		entityGroup_.entity[i].gameObject.Initialize();
		entityGroup_.entity[i].colliderState.Initialize(entityGroup_.entity[i].gameObject, entityGroup_.entity[i].physicsData, entityGroup_.entity[i].gameObject.transformData.scale);
		entityGroup_.entity[i].collider.owner = &entityGroup_.entity[i].colliderState;
		entityGroup_.entity[i].collider.isEnabled = true;
		entityGroup_.entity[i].collider.isTrigger = false;
		entityGroup_.entity[i].collider.bodyType = BodyType::kStatic;
		entityGroup_.entity[i].collider.layer = Layer::kGround;
		entityGroup_.entity[i].collider.maskLayer = static_cast<uint32_t>(Layer::kPlayer);
		entityGroup_.entity[i].collider.onCollision = [this, i](ColliderState* other) {this->OnCollision(i, other); };
	}
}

//更新
void BaseGround::Update() {
	//ゲームオブジェクトなどの設定
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.entity[i].physicsData.velocity += entityGroup_.entity[i].physicsData.acceleration * Math::kDeltaTime;
		entityGroup_.entity[i].gameObject.transformData.translate += entityGroup_.entity[i].physicsData.velocity * Math::kDeltaTime;
		entityGroup_.renderObject.object3d->SetGameObject(i, entityGroup_.entity[i].gameObject);
		entityGroup_.renderObject.hitBox->SetTransformData(i, entityGroup_.entity[i].gameObject.transformData);
	}

	//描画オブジェクトの更新
	entityGroup_.renderObject.object3d->Update();
	entityGroup_.renderObject.hitBox->Update();
}

//デバッグ
void BaseGround::Debug() {
	ImGuiManager::TreeNodeForEntityGroup(entityGroup_.modelName, entityGroup_);
}

//描画
void BaseGround::Draw() {
	entityGroup_.renderObject.object3d->Draw();
	entityGroup_.renderObject.hitBox->Draw();
}

//衝突したら
void BaseGround::OnCollision(uint32_t index, ColliderState* other) {
}

//カメラのセッター
void BaseGround::SetCamera(Camera* camera) {
	entityGroup_.renderObject.object3d->SetCamera(camera);
	entityGroup_.renderObject.hitBox->SetCamera(camera);
}

//エンティティのゲッター
std::vector<Entity>& BaseGround::GetEntity() {
	// TODO: return ステートメントをここに挿入します
	return entityGroup_.entity;
}

