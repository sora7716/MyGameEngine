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
#include <algorithm>

Player::PlayerPose Player::PlayerPose::Lerp(const PlayerPose& playerPose1, const PlayerPose& playerPose2, float t){
	PlayerPose result = {};
	result.root = Transform::Lerp(playerPose1.root, playerPose2.root, t);
	result.head = Transform::Lerp(playerPose1.head, playerPose2.head, t);
	result.body = Transform::Lerp(playerPose1.body, playerPose2.body, t);
	result.leftArm = Transform::Lerp(playerPose1.leftArm, playerPose2.leftArm, t);
	result.rightArm = Transform::Lerp(playerPose1.rightArm, playerPose2.rightArm, t);
	return result;
}

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

		//振る舞いを初期化
		switch (behavior_){
		case Behavior::kNormal:
			//通常
			InitializeNormal();
			break;
		case Behavior::kMove:
			//移動
			InitializeMoving();
			break;
		}
	}

	//振る舞いを更新
	switch (behavior_){
	case Behavior::kNormal:
		//通常
		UpdateNormal();
		break;
	case Behavior::kMove:
		//移動
		UpdateMoving();
		break;
	case Behavior::kAttack:
		//攻撃フェーズのリクエストがあったら
		if (attackPhaseRequest_ != AttackPhase::kCount){
			//攻撃フェーズを変更
			attackPhase_ = attackPhaseRequest_;
			//リクエストをリセット
			attackPhaseRequest_ = AttackPhase::kCount;

			//初期化
			switch (attackPhase_){
			case Player::AttackPhase::kWindup:
				//振りかぶり
				InitializeWindup();
				break;
			case Player::AttackPhase::kSwing:
				//振り下げ
				InitializeSwing();
				break;
			}
		}

		//更新
		switch (attackPhase_){
		case Player::AttackPhase::kWindup:
			//振りかぶり
			UpdateWindup();
			break;
		case Player::AttackPhase::kSwing:
			//振り下げ
			UpdateSwing();
			break;
		}
		break;
	}

	//ポーズを適応
	object3d_->SetNodeLocalTransform("Player_Root", currentPose_.root);
	object3d_->SetNodeLocalTransform("Player_Root/Head", currentPose_.head);
	object3d_->SetNodeLocalTransform("Player_Root/Body", currentPose_.body);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_L", currentPose_.leftArm);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_R", currentPose_.rightArm);
}

//デバッグでImGuiを使用できるようにする
void Player::DebugImGui(){
#ifdef USE_IMGUI
	ImGui::Begin("player");
	ImGui::SeparatorText("Root");
	ImGui::DragFloat("root.amplitude,", &movingRootAmplitude_, 0.01f);
	ImGui::DragFloat("root.speed,", &movingRootSpeed_, 0.01f);
	ImGui::SeparatorText("Arm");
	ImGui::DragFloat("arm.amplitude,", &movingArmAmplitude_, 0.01f);
	ImGui::DragFloat("arm.speed,", &movingArmSpeed_, 0.01f);

	if (behavior_ == Behavior::kNormal){
		ImGui::Text("normal");
	} else if(behavior_ == Behavior::kMove){
		ImGui::Text("move");
	} else{
		ImGui::Text("attack");
	}
	ImGui::End();
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
	Matrix4x4 rotMat = matrixUtility::MakeRotateMatrix(cameraObject_->GetTransform().rotate);

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

//過去のポーズを設定
void Player::SettingPreviousPose(){
	prePose_.root = object3d_->GetNodeLocalTransform("Player_Root");
	prePose_.head = object3d_->GetNodeLocalTransform("Player_Root/Head");
	prePose_.body = object3d_->GetNodeLocalTransform("Player_Root/Body");
	prePose_.rightArm = object3d_->GetNodeLocalTransform("Player_Root/Arm_R");
	prePose_.leftArm = object3d_->GetNodeLocalTransform("Player_Root/Arm_L");
}

//モーション遷移の初期化
void Player::InitializeTransition(){
	//切り替え用のタイマーをリセット
	transitionTimer_ = 0.0f;

	//過去のポーズを設定
	SettingPreviousPose();
}

//モーション遷移の更新
void Player::UpdateTransition(PlayerPose targetPose){
	//切り替え用のタイマー
	transitionTimer_ += mathUtility::kDeltaTime;
	//ブレンドする割合
	float blendRate = std::clamp(transitionTimer_ / kTransitionDuration, 0.0f, 1.0f);
	//目標の位置まで補間
	currentPose_ = PlayerPose::Lerp(prePose_, targetPose, blendRate);
}

//通常状態の初期化
void Player::InitializeNormal(){
	//小刻みに揺れるタイマーをリセット
	bobTimer_ = 0.0f;

	//モーション遷移の初期化
	InitializeTransition();
}

//通常状態の更新
void Player::UpdateNormal(){
	//時間を計測
	bobTimer_ += mathUtility::kDeltaTime;

	//プレイヤーの目標ポーズ
	PlayerPose targetPose = currentPose_;

	//上下に小刻みに動く
	targetPose.root.SetEulerAngle(Vector3::GetZero());
	targetPose.root.translate.y = std::sin(bobTimer_ * bobRootSpeed_) * bobRootAmplitude_;

	//体は左右に小刻みに動く
	Vector3 bodyEulerAngle = Vector3::GetZero();
	bodyEulerAngle.z = std::sin(bobTimer_ * bobBodySpeed_) * bobBodyAmplitude_;
	targetPose.body.SetEulerAngle(bodyEulerAngle);

	//両腕を動かす
	Vector3 armEulerAngle = Vector3::GetZero();
	armEulerAngle.x = std::sin(bobTimer_ * bobArmSpeed_) * bobArmAmplitude_;
	targetPose.leftArm.SetEulerAngle(armEulerAngle);
	targetPose.rightArm.SetEulerAngle(armEulerAngle);

	//モーション遷移の更新
	UpdateTransition(targetPose);
}

//移動状態の初期化
void Player::InitializeMoving(){
	//移動タイマーのリセット
	movingTimer_ = 0.0f;

	//モーション遷移の初期化
	InitializeTransition();
}

//移動状態の更新
void Player::UpdateMoving(){
	//時間を計測
	movingTimer_ += mathUtility::kDeltaTime;

	//プレイヤーの目標ポーズ
	PlayerPose targetPose = currentPose_;

	//上下に小刻みに動く
	targetPose.root.translate.y = std::sin(movingTimer_ * movingRootSpeed_) * movingRootAmplitude_;

	//全体的に少し前傾姿勢
	Vector3 rootEulerAngle = Vector3::GetZero();
	rootEulerAngle.x = 0.3f;
	targetPose.root.SetEulerAngle(rootEulerAngle);

	//両腕を動かす
	Vector3 armEulerAngle = Vector3::GetZero();
	armEulerAngle.x = std::sin(movingTimer_ * movingArmSpeed_) * movingArmAmplitude_;
	targetPose.leftArm.SetEulerAngle(armEulerAngle);
	targetPose.rightArm.SetEulerAngle(-armEulerAngle);

	//モーション遷移の更新
	UpdateTransition(targetPose);
}

//振りかぶり状態の初期化
void Player::InitializeWindup(){
	//振り上げモーションのタイマーの初期化
	windupTimer_ = 0.0f;

	//過去のポーズを設定
	SettingPreviousPose();
}

//振りかぶり状態の更新
void Player::UpdateWindup(){
	//プレイヤーの目標ポーズ
	PlayerPose targetPose = currentPose_;

	//左手を振り上げる
	targetPose.leftArm.SetEulerAngleDegrees({ 0.0f, 135.0f, 135.0f });

	//体全体をねじる
	targetPose.root.SetEulerAngleDegrees({ 0.0f,60.0f,0.0f });

	//切り替え用のタイマー
	windupTimer_ += mathUtility::kDeltaTime;
	//係数
	float t = std::clamp(windupTimer_ / kWindupDuration, 0.0f, 1.0f);
	//目標の位置まで補間
	currentPose_ = PlayerPose::Lerp(prePose_, targetPose, t);

	//フェーズを切り替える
	if (windupTimer_ > kWindupDuration){
		attackPhaseRequest_ = AttackPhase::kSwing;
	}
}

//振り下ろし状態の初期化
void Player::InitializeSwing(){
	//タイマーリセット
	swingTimer_ = 0.0f;
	//ポーズを保存
	SettingPreviousPose();
}

//振り下ろし状態の更新
void Player::UpdateSwing(){
	//プレイヤーの目標ポーズ
	PlayerPose targetPose = currentPose_;
	//切り替え用のタイマー
	swingTimer_ += mathUtility::kDeltaTime;

	//左手を振り上げる
	targetPose.leftArm.SetEulerAngleDegrees({ 220.0f, -60.0f, 0.0f });

	//体全体をねじる
	targetPose.root.SetEulerAngleDegrees({ 0.0f,-60.0f,0.0f });

	//係数
	float t = std::clamp(swingTimer_ / kSwingDuration, 0.0f, 1.0f);
	//目標の位置まで補間
	currentPose_ = PlayerPose::Lerp(prePose_, targetPose, t);

	//フェーズを切り替える
	if (swingTimer_ > kSwingDuration){
		//振る舞いを変更
		behaviorRequest_ = Behavior::kNormal;
		//攻撃フェーズをリセット
		attackPhaseRequest_ = AttackPhase::kWindup;
	}
}
