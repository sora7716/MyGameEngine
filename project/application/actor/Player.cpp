#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "GameObject.h"
#include "MathUtility.h"
#include "matrixUtility.h"
#include "Camera.h"

//コンストラクタ
Player::Player(GameObject* gameObject, Input& input)
	:Component(gameObject), input_(input){
}

//デストラクタ
Player::~Player(){
}

//初期化
void Player::Initialize(){
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();

	//SRTの調整
	gameObject_->GetTransform().translate = { 0.0f,1.0f,0.0f };
}

//更新
void Player::Update(){
	//加速度をリセット
	acceleration_ = {};

	//移動の操作
	MoveControl();
	//ジャンプの操作
	JumpControl();

	//重力を適応
	ApplyGravity();
	//速度を位置へ反映する
	Movement();

	//移動方向に向かせる
	LookAt();

	//地面との接地
	ResolveGround();
}

//複製
std::unique_ptr<Component> Player::Clone(GameObject* gameObject) const{
	std::unique_ptr<Player>cloneInstance = std::make_unique<Player>(gameObject, this->input_);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//カメラのオブジェクトの設定
void Player::SetCameraObject(GameObject* cameraObject){
	cameraObject_ = cameraObject;
}

//移動の操作
void Player::MoveControl(){
	//横移動
	if (input_.PressKey(DIK_A)){
		inputDirection_.x = -1.0f;
	} else if (input_.PressKey(DIK_D)){
		inputDirection_.x = 1.0f;
	} else{
		inputDirection_.x = 0.0f;
	}

	//縦移動
	if (input_.PressKey(DIK_W)){
		inputDirection_.z = 1.0f;
	} else if (input_.PressKey(DIK_S)){
		inputDirection_.z = -1.0f;
	} else{
		inputDirection_.z = 0.0f;
	}

	if (inputDirection_.LengthSquared() > 0.0f){
		inputDirection_ = inputDirection_.Normalize();
	}

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = matrixUtility::MakeRotateMatrix(cameraObject_->GetTransform().quaternion);

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
	acceleration_.x = (targetVelocity.x - velocity_.x) * kMoveResponse;
	acceleration_.z = (targetVelocity.z - velocity_.z) * kMoveResponse;
}

//ジャンプの操作
void Player::JumpControl(){
	if (input_.TriggerKey(DIK_SPACE)){
		if (isOnGround_){
			velocity_.y = kJumpSpeed;
			isOnGround_ = false;
		}
	}
}

//重力を適応
void Player::ApplyGravity(){
	//重力を追加
	acceleration_.y += kGravity;
}

//速度を位置へ反映する
void Player::Movement(){
	//Transformの取得
	Transform& transform = gameObject_->GetTransform();
	//速度に加速度を反映
	velocity_ += acceleration_ * mathUtility::kDeltaTime;
	//平行移動に速度を反映
	transform.translate += velocity_ * mathUtility::kDeltaTime;
}

//地面との接触
void Player::ResolveGround(){
	//Transformの取得
	Transform& transform = gameObject_->GetTransform();

	//地面のある位置
	constexpr float kGravityY = 1.0f;

	//y座標の0より下に行かないようにする
	transform.translate.y = std::max(transform.translate.y, kGravityY);

	//地面についたら
	if (gameObject_->GetTransform().translate.y <= kGravityY){
		transform.translate.y = kGravityY;

		if (velocity_.y > 0.0f){
			velocity_.y = 0.0f;
		}

		isOnGround_ = true;
	}
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
	Quaternion targetQuaternion = Quaternion::MakeQuaternionForEulerAngle({ 0.0f,yaw,0.0f });

	//目標のクォータニオンの方向に向かせる
	transform.quaternion = Quaternion::Slerp(transform.quaternion, targetQuaternion, kLookAtSpeed * mathUtility::kDeltaTime);
}
