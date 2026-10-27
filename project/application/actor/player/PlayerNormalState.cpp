#include "PlayerNormalState.h"
#include "MathUtility.h"
#include "Object3d.h"
#include <cassert>

//コンストラクタ
PlayerNormalState::PlayerNormalState(){
}

//デストラクタ
PlayerNormalState::~PlayerNormalState(){
}

//初期化
void PlayerNormalState::Enter(){
	//通常状態の初期化
	InitializeNormal();
}

//更新
void PlayerNormalState::Update(){
	//通常状態の更新
	UpdateNormal();
	
	//ポーズを適応
	object3d_->SetNodeLocalTransform("Player_Root", currentPose_.root);
	object3d_->SetNodeLocalTransform("Player_Root/Body", currentPose_.body);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_L", currentPose_.leftArm);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_R", currentPose_.rightArm);
}

//終了
void PlayerNormalState::Exit(){
}

//通常状態の初期化
void PlayerNormalState::InitializeNormal(){
	//小刻みに揺れるタイマーをリセット
	bobTimer_ = 0.0f;

	//モーション遷移の初期化
	InitializeTransition();
}

//通常状態の更新
void PlayerNormalState::UpdateNormal(){
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