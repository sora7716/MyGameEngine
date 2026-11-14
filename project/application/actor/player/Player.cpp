#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "matrixUtility.h"
#include "BaseScene.h"
#include "RigidBody.h"
#include "BaseCollider.h"
#include "Object3d.h"
#include "PlayerNormalState.h"
#include "PlayerMovementState.h"
#include "PlayerAttackState.h"

//コンストラクタ
Player::Player(GameObject* gameObject)
	:Component(gameObject) {
}

//デストラクタ
Player::~Player() {
}

//初期化
void Player::Initialize() {
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();

	//ステートの初期化
	states_[static_cast<uint32_t>(Behavior::kNormal)] = std::make_unique<PlayerNormalState>();
	states_[static_cast<uint32_t>(Behavior::kMove)] = std::make_unique<PlayerMovementState>();
	states_[static_cast<uint32_t>(Behavior::kAttack)] = std::make_unique<PlayerAttackState>();
}

//更新
void Player::Update() {
	//移動方向に向かせる
	LookAt();

	//移動キーのどれかが押されてるか確認
	bool isMoveInputActive = input_->PressKey(DIK_A) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S);

	//移動キーが押されているか確認
	if(behavior_ != Behavior::kAttack) {
		if(isMoveInputActive) {
			//現在の振る舞いを確認
			if(behavior_ != Behavior::kMove) {
				behaviorRequest_ = Behavior::kMove;
			}
		} else {
			//現在の振る舞いを確認
			if(behavior_ != Behavior::kNormal) {
				behaviorRequest_ = Behavior::kNormal;
			}
		}
	}

	//攻撃キーが押されているか
	bool isAttackInputActive = input_->TriggerMouseButton(Click::kLeft);
	if(isAttackInputActive) {
		if(behavior_ != Behavior::kAttack) {
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	//振る舞いのリクエストがあったら
	if(behaviorRequest_ != Behavior::kCount) {
		//振る舞いを変更
		behavior_ = behaviorRequest_;
		//リクエストをリセット
		behaviorRequest_ = Behavior::kCount;

		//現在のステート初期化
		IPlayerState* newState = states_[static_cast<uint32_t>(behavior_)].get();
		if(currentState_ != newState) {
			//現在のステートを終了
			if(currentState_) {
				currentState_->Exit();
			}
			newState->Setup(this,object3d_);
			newState->Enter();
			currentState_ = newState;
		}
	}

	if(currentState_) {
		//現在のステートの更新
		currentState_->Update();
	}
}

//デバッグでImGuiを使用できるようにする
void Player::DebugImGui() {
#ifdef USE_IMGUI
#endif // USE_IMGUI
}

//複製
std::unique_ptr<Component> Player::Clone(GameObject* gameObject) const {
	std::unique_ptr<Player>cloneInstance = std::make_unique<Player>(gameObject);

	//初期化
	cloneInstance->Initialize();
	cloneInstance->SetInput(input_);

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//ゲームオブジェクトから解除する
void Player::OnGameObjectRemoving(GameObject* target) {
	if(cameraObject_ == target) {
		cameraObject_ = nullptr;
	}
}

//カメラのオブジェクトの設定
void Player::SetCameraObject(GameObject* cameraObject) {
	cameraObject_ = cameraObject;
}

//振る舞いのリクエストの設定
void Player::SetBehaviorRequest(Behavior request) {
	behaviorRequest_ = request;
}

//オブジェクト3dの設定
void Player::SetObject3d(Object3d* object3d) {
	assert(object3d);
	object3d_ = object3d;
}

//入寮区の設定
void Player::SetInput(Input* input) {
	assert(input);
	input_ = input;
}

//コンポーネントのタイプ名を取得
std::string Player::GetTypeName() const {
	return "Player";
}

//移動方向に向かせる
void Player::LookAt() {
	Transform& transform = gameObject_->GetTransform();

	//長さが十分に大きくないと
	if(worldDirection_.LengthSquared() < 0.001f) {
		return;
	}

	//方向ベクトルからヨーを取得
	float yaw = std::atan2(worldDirection_.x,worldDirection_.z);

	//目標のクォータニオンを作成
	Quaternion targetQuaternion = Quaternion::EulerAngleToQuaternion({0.0f,yaw,0.0f});

	//目標のクォータニオンの方向に向かせる
	transform.rotate = Quaternion::Slerp(transform.rotate,targetQuaternion,kLookAtSpeed * mathUtility::kDeltaTime);
}