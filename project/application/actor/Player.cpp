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
#include "ImGuiManager.h"

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
}

//更新
void Player::Update(){
	//移動の操作
	MoveControl();
	//ジャンプの操作
	JumpControl();

	//移動方向に向かせる
	LookAt();

	RootUpdate();
}

//デバッグでImGuiを使用できるようにする
void Player::DebugImGui(){
	ImGui::Begin("player");
	ImGui::End();
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
	Quaternion targetQuaternion = Quaternion::MakeQuaternionForEulerAngle({ 0.0f,yaw,0.0f });

	//目標のクォータニオンの方向に向かせる
	transform.quaternion = Quaternion::Slerp(transform.quaternion, targetQuaternion, kLookAtSpeed * mathUtility::kDeltaTime);
}

//通常状態の初期化
void Player::RootInitialize(){

}

//通常状態の更新
void Player::RootUpdate(){
	Transform playerRoot = object3d_->GetNodeLocalTransform("Player_Root");
	float amplitude = 1.0f;
	static float t = 0.0f;
	t += mathUtility::kDeltaTime;
	playerRoot.translate.y = std::sin(t * 2.0f) * amplitude;
	object3d_->SetNodeLocalTransform("Player_Root", playerRoot);
}

//移動状態の初期化
void Player::MoveInitialize(){
}

//移動状態の更新
void Player::MoveUpdate(){
}

//攻撃状態の初期化
void Player::AttackInitialize(){
}

//攻撃状態の更新
void Player::AttackUpdate(){
}
