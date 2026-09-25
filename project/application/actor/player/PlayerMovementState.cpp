#include "PlayerMovementState.h"
#include "MathUtility.h"
#include "Object3d.h"
#include <cassert>

//コンストラクタ
PlayerMovementState::PlayerMovementState(){
}

//デストラクタ
PlayerMovementState::~PlayerMovementState(){
}

//初期化
void PlayerMovementState::Enter(){
	//移動状態の初期化
	InitializeMoving();
}

//更新
void PlayerMovementState::Update(){
	//移動状態の更新
	UpdateMoving();

	//ポーズを適応
	object3d_->SetNodeLocalTransform("Player_Root", currentPose_.root);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_L", currentPose_.leftArm);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_R", currentPose_.rightArm);
}

//終了
void PlayerMovementState::Exit(){
}

//移動状態の初期化
void PlayerMovementState::InitializeMoving(){
	//移動タイマーのリセット
	movingTimer_ = 0.0f;

	//モーション遷移の初期化
	InitializeTransition();
}

//移動状態の更新
void PlayerMovementState::UpdateMoving(){
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
