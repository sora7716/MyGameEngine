#include "Player.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "ImGuiManager.h"

//コンストラクタ
Player::Player() {

}

//デストラクタ
Player::~Player() {

}

//初期化
void Player::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	entityGroup_.objectCount = 1;
	entityGroup_.modelName = "player";
	entityGroup_.renderObject = entityGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	entityGroup_.entity.resize(entityGroup_.objectCount);

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
		entity.collider.bodyType = BodyType::kDynamic;
		entity.collider.layer = Layer::kPlayer;
		entity.collider.maskLayer = static_cast<uint32_t>(Layer::kGround);
	}
}

//更新
void Player::Update() {
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.renderObject.object3d->SetGameObject(i, entityGroup_.entity[i].gameObject);
		entityGroup_.renderObject.hitBox->SetTransformData(i, entityGroup_.entity[i].gameObject.transformData);
	}

	entityGroup_.renderObject.object3d->Update();
	entityGroup_.renderObject.hitBox->Update();
}

//デバッグ
void Player::Debug() {
	ImGuiManager::TreeNodeForEntityGroup("player", entityGroup_);
}

//描画
void Player::Draw() {
	entityGroup_.renderObject.object3d->Draw();
	entityGroup_.renderObject.hitBox->Draw();
}

//カメラのセッター
void Player::SetCamera(Camera* camera) {
	entityGroup_.renderObject.object3d->SetCamera(camera);
	entityGroup_.renderObject.hitBox->SetCamera(camera);
}

//エンティティのゲッター
std::vector<Entity>& Player::GetEntity() {
	// TODO: return ステートメントをここに挿入します
	return entityGroup_.entity;
}
