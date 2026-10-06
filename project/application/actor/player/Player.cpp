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
	:Component(gameObject){
}

//デストラクタ
Player::~Player(){
}

//初期化
void Player::Initialize(){
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
	//現在接続しているシーンを取得
	BaseScene* currentScene = gameObject_->GetCurrentScene();
	//入力の取得
	input_ = currentScene->GetSceneContext().input;

	//SRTの調整
	gameObject_->GetTransform().translate = { 0.0f,1.0f,0.0f };

	//リジッドボディを受け取る
	rigidBody_ = gameObject_->GetComponent<RigidBody>();

	//オブジェクト3dを受け取る
	object3d_ = gameObject_->GetComponent<Object3d>();

	//ステートの初期化
	states_[static_cast<uint32_t>(Behavior::kNormal)] = std::make_unique<PlayerNormalState>();
	states_[static_cast<uint32_t>(Behavior::kMove)] = std::make_unique<PlayerMovementState>();
	states_[static_cast<uint32_t>(Behavior::kAttack)] = std::make_unique<PlayerAttackState>();
}

//更新
void Player::Update(){
	//動かす
	MoveControl();
	//ジャンプの操作
	JumpControl();

	//移動方向に向かせる
	LookAt();

	//移動キーのどれかが押されてるか確認
	bool isMoveInputActive = input_->PressKey(DIK_A) ||
		input_->PressKey(DIK_D) ||
		input_->PressKey(DIK_W) ||
		input_->PressKey(DIK_S);

	//移動キーが押されているか確認
	if (behavior_ != Behavior::kAttack){
		if (isMoveInputActive){
			//現在の振る舞いを確認
			if (behavior_ != Behavior::kMove){
				behaviorRequest_ = Behavior::kMove;
			}
		} else{
			//現在の振る舞いを確認
			if (behavior_ != Behavior::kNormal){
				behaviorRequest_ = Behavior::kNormal;
			}
		}
	}

	//攻撃キーが押されているか
	bool isAttackInputActive = input_->TriggerMouseButton(Click::kLeft);
	if (isAttackInputActive){
		if (behavior_ != Behavior::kAttack){
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	//振る舞いのリクエストがあったら
	if (behaviorRequest_ != Behavior::kCount){
		//振る舞いを変更
		behavior_ = behaviorRequest_;
		//リクエストをリセット
		behaviorRequest_ = Behavior::kCount;

		//現在のステート初期化
		currentState_ = states_[static_cast<uint32_t>(behavior_)].get();
		currentState_->Setup(this, object3d_);
		currentState_->Enter();
	}

	//現在のステートの更新
	currentState_->Update();
}

//デバッグでImGuiを使用できるようにする
void Player::DebugImGui(){
#ifdef USE_IMGUI
#endif // USE_IMGUI
}

//複製
std::unique_ptr<Component> Player::Clone(GameObject* gameObject) const{
	std::unique_ptr<Player>cloneInstance = std::make_unique<Player>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//ゲームオブジェクトから解除する
void Player::OnGameObjectRemoving(GameObject* target){
	if (cameraObject_ == target){
		cameraObject_ = nullptr;
	}
}

//衝突したら
void Player::OnCollisionStay(const CollisionInfo& info){
	//リジッドボディと衝突対象のコライダーのどちらかが沿うん材していない場合
	if (!rigidBody_ || !info.other){
		return;
	}
}

//カメラのオブジェクトの設定
void Player::SetCameraObject(GameObject* cameraObject){
	cameraObject_ = cameraObject;
}

//振る舞いのリクエストの設定
void Player::SetBehaviorRequest(Behavior request){
	behaviorRequest_ = request;
}

//オブジェクト3dの設定
void Player::SetObject3d(Object3d* object3d){
	object3d_ = object3d;
}

//移動の操作
void Player::MoveControl(){
	//横移動
	if (input_->PressKey(DIK_A)){
		inputDirection_.x = -1.0f;
	} else if (input_->PressKey(DIK_D)){
		inputDirection_.x = 1.0f;
	} else{
		inputDirection_.x = 0.0f;
	}

	//縦移動
	if (input_->PressKey(DIK_W)){
		inputDirection_.z = 1.0f;
	} else if (input_->PressKey(DIK_S)){
		inputDirection_.z = -1.0f;
	} else{
		inputDirection_.z = 0.0f;
	}

	if (inputDirection_.LengthSquared() > 0.0f){
		inputDirection_ = inputDirection_.Normalize();
	}

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Matrix4x4::Identity4x4();
	if (cameraObject_){
		rotMat = matrixUtility::MakeRotateMatrix(cameraObject_->GetTransform().rotate);
	}

	//カメラの向いてる方向を正にする(XとZ軸限定)
	worldDirection_ = mathUtility::TransformNormal(inputDirection_, rotMat);
	//Y軸は考えない
	worldDirection_.y = 0.0f;

	//長さが0より大きければ
	if (worldDirection_.LengthSquared() > 0.0f){
		worldDirection_ = worldDirection_.Normalize();
	}

	//目標速度を決定
	const Vector3 targetVelocity = worldDirection_ * kMoveSpeed;

	//水平移動の加速度を目標速度に近づける
	Vector3& velocity = rigidBody_->GetVelocity();
	Vector3& acceleration = rigidBody_->GetAcceleration();
	acceleration.x = (targetVelocity.x - velocity.x) * kMoveResponse;
	acceleration.z = (targetVelocity.z - velocity.z) * kMoveResponse;
}

//ジャンプの操作
void Player::JumpControl(){
	if (!input_->TriggerKey(DIK_SPACE)){
		return;
	}

	if (!rigidBody_->IsOnGround()){
		return;
	}

	rigidBody_->GetVelocity().y = kJumpSpeed;
}

//移動方向に向かせる
void Player::LookAt(){
	Transform& transform = gameObject_->GetTransform();

	//長さが十分に大きくないと
	if (worldDirection_.LengthSquared() < 0.001f){
		return;
	}

	//方向ベクトルからヨーを取得
	float yaw = std::atan2(worldDirection_.x, worldDirection_.z);

	//目標のクォータニオンを作成
	Quaternion targetQuaternion = Quaternion::EulerAngleToQuaternion({ 0.0f,yaw,0.0f });

	//目標のクォータニオンの方向に向かせる
	transform.rotate = Quaternion::Slerp(transform.rotate, targetQuaternion, kLookAtSpeed * mathUtility::kDeltaTime);
}