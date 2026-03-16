#include "Player.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "ImGuiManager.h"
#include "algorithm/Physics.h"
#include "algorithm/Math.h"
#include "Input.h"
#include "Camera.h"

//コンストラクタ
Player::Player() {

}

//デストラクタ
Player::~Player() {

}

//初期化
void Player::Initialize(Object3dCommon* object3dCommon, Camera* camera, Input* input) {
	input_ = input;
	camera_ = camera;
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
		entity.physicsData.acceleration.y = Physics::kGravity;
		entity.colliderState.Initialize(entity.gameObject, entity.physicsData, entity.gameObject.transformData.scale);
		entity.collider.owner = &entity.colliderState;
		entity.collider.isEnabled = true;
		entity.collider.isTrigger = false;
		entity.collider.bodyType = BodyType::kDynamic;
		entity.collider.layer = Layer::kPlayer;
		entity.collider.maskLayer = static_cast<uint32_t>(Layer::kGround);
		entity.collider.onCollision = [this](ColliderState* other) {this->OnCollision(other); };
	}
}

//更新
void Player::Update() {
	Move();
	Jump();
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		entityGroup_.entity[i].physicsData.velocity += entityGroup_.entity[i].physicsData.acceleration * Math::kDeltaTime;
		entityGroup_.entity[i].gameObject.transformData.translate += entityGroup_.entity[i].physicsData.velocity * Math::kDeltaTime;

		entityGroup_.renderObject.object3d->SetGameObject(i, entityGroup_.entity[i].gameObject);
		entityGroup_.renderObject.hitBox->SetTransformData(i, entityGroup_.entity[i].gameObject.transformData);

		//プレイヤーが落ちたら
		if (entityGroup_.entity[i].gameObject.transformData.translate.y < -10.0f) {
			entityGroup_.entity[i].gameObject.isAlive = false;
		}
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

//平行移動のゲッター
Vector3 Player::GetTranslate() {
	return entityGroup_.entity[0].gameObject.transformData.translate;
}

//ゴールしたかどうか
bool Player::IsGoalReached() {
	return isGoalReached_;
}

//移動
void Player::Move() {
#ifdef _DEBUG
	//前後
	if (input_->PressKey(DIK_UP)) {
		moveDirection_.z = 1.0f;
	} else if (input_->PressKey(DIK_DOWN)) {
		moveDirection_.z = -1.0f;
	} else {
		moveDirection_.z = 0.0f;
	}

	//左右
	if (input_->PressKey(DIK_RIGHT)) {
		moveDirection_.x = 1.0f;
	} else if (input_->PressKey(DIK_LEFT)) {
		moveDirection_.x = -1.0f;
	} else {
		moveDirection_.x = 0.0f;
	}
#endif // _DEBUG

	//XboxPadの平行移動
	if (input_->IsXboxPadConnected(xboxNumber_)) {
		if (std::fabs(input_->GetXboxPadLeftStick(xboxNumber_).x) > 0.0f
			|| std::fabs(input_->GetXboxPadLeftStick(xboxNumber_).y) > 0.0f) {
			moveDirection_.x = input_->GetXboxPadLeftStick(xboxNumber_).x;
			moveDirection_.z = input_->GetXboxPadLeftStick(xboxNumber_).y;
		} else {
			moveDirection_.x = 0.0f;
			moveDirection_.z = 0.0f;
		}
	}

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateMatrix((camera_->GetQuaternion()));

	//カメラの向いてる方向を正にする(XとZ軸限定)
	Vector3 moveDirXZ = Math::TransformNormal(Vector3(moveDirection_.x, 0.0f, moveDirection_.z), rotMat);

	//Y軸のそのまま
	moveDirection_ = { moveDirXZ.x,moveDirection_.y,moveDirXZ.z };

	//移動させる
	entityGroup_.entity[0].physicsData.velocity.x = moveDirection_.x * kMoveSpeed;
	entityGroup_.entity[0].physicsData.velocity.z = moveDirection_.z * kMoveSpeed;
}

//ジャンプ
void Player::Jump() {
	if (entityGroup_.entity[0].physicsData.isOnGround) {
		//地面にいたらジャンプできるようにする
		if (input_->TriggerXboxPad(xboxNumber_, XboxInput::kA)) {
			//Y軸に初速を代入
			entityGroup_.entity[0].physicsData.velocity.y = kJumpSpeed;
			//地面にいるかどうかのフラグをfalse
			entityGroup_.entity[0].physicsData.isOnGround = false;
		}

#ifdef _DEBUG
		if (input_->TriggerKey(DIK_SPACE)) {
			//Y軸に初速を代入
			entityGroup_.entity[0].physicsData.velocity.y = kJumpSpeed;
			//地面にいるかどうかのフラグをfalse
			entityGroup_.entity[0].physicsData.isOnGround = false;
		}
#endif // _DEBUG

	}
}

//衝突したら
void Player::OnCollision(ColliderState* other) {
	if (other->tag == Tag::kGoal) {
		isGoalReached_ = true;
	}
}
