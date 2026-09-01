#include "Player.h"
#include "Input.h"
#include "GameObject.h"

//コンストラクタ
Player::Player(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
Player::~Player(){
}

//初期化
void Player::Initialize(){
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
}

//更新
void Player::Update(){
	//移動
	gameObject_->GetTransform().translate += velocity_;

	//横移動
	if (input_->PressKey(DIK_A)){
		velocity_.x = kSpeed;
	} else if (input_->PressKey(DIK_D)){
		velocity_.x = -kSpeed;
	} else{
		velocity_.x = 0.0f;
	}

	//縦移動
	if (input_->PressKey(DIK_W)){
		velocity_.z = kSpeed;
	} else if (input_->PressKey(DIK_S)){
		velocity_.z = -kSpeed;
	} else{
		velocity_.z = 0.0f;
	}
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

//入力の取得
void Player::SetInput(Input* input){
	input_ = input;
}
