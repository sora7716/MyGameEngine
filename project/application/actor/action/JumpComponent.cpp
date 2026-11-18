#include "JumpComponent.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
JumpComponent::JumpComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
JumpComponent::~JumpComponent() {
}

//セットアップ
void JumpComponent::Setup(RigidBody* rigidBody,float jumpSpeed) {
	//リジッドボディの記録
	assert(rigidBody);
	rigidBody_ = rigidBody;
	//ジャンプの速度を記録
	jumpSpeed_ = jumpSpeed;
}

//初期化
void JumpComponent::Initialize() {
}

//更新
void JumpComponent::Update() {
	//リジッドボディが無ければ
	if(!rigidBody_) {
		Logger::OutputLog("JumpComponentにRigidBodyが設定されていません");
		return;
	}

	//ジャンプを要求してない場合
	if(!jumpRequest_) {
		return;
	}

	//地面についていなければ
	if(!rigidBody_->IsOnGround()) {
		jumpRequest_ = false;
		return;
	}

	//ジャンプする初速を設定
	rigidBody_->GetVelocity().y = jumpSpeed_;
}

//プロパティ一覧
void JumpComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("jumpSpeed",jumpSpeed_);
}

//複製
std::unique_ptr<Component> JumpComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<JumpComponent>cloneInstance = std::make_unique<JumpComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->rigidBody_ = this->rigidBody_;
	cloneInstance->jumpSpeed_ = this->jumpSpeed_;
	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string JumpComponent::GetTypeName() const {
	return "JumpComponent";
}

//ジャンプする要求を設定
void JumpComponent::SetJumpRequest(bool request) {
	jumpRequest_ = request;
}
