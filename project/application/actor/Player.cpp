#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "GameObject.h"
#include "MathUtility.h"

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
	gameObject_->GetTransform().scale = { 0.5f,0.5f,0.5f };
	gameObject_->GetTransform().translate = { 0.0f,1.0f,0.0f };
}

//更新
void Player::Update(){
	//移動の操作
	MoveControl();
	//ジャンプの操作
	JumpControl();

	//重力を追加
	velocity_.y += kGravity * mathUtility::kDeltaTime;

	//Transformの取得
	Transform& transform = gameObject_->GetTransform();
	//平行移動に速度を反映
	transform.translate += velocity_ * mathUtility::kDeltaTime;

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

//複製
std::unique_ptr<Component> Player::Clone(GameObject* gameObject) const{
	std::unique_ptr<Player>cloneInstance = std::make_unique<Player>(gameObject, this->input_);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//移動の操作
void Player::MoveControl(){
	//横移動
	if (input_.PressKey(DIK_A)){
		velocity_.x = -kMoveSpeed;
	} else if (input_.PressKey(DIK_D)){
		velocity_.x = kMoveSpeed;
	} else{
		velocity_.x = 0.0f;
	}

	//縦移動
	if (input_.PressKey(DIK_W)){
		velocity_.z = kMoveSpeed;
	} else if (input_.PressKey(DIK_S)){
		velocity_.z = -kMoveSpeed;
	} else{
		velocity_.z = 0.0f;
	}
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
