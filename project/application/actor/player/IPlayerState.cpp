#include "IPlayerState.h"
#include "Object3d.h"
#include "MathUtility.h"

//補間
IPlayerState::PlayerPose IPlayerState::PlayerPose::Lerp(const PlayerPose& playerPose1, const PlayerPose& playerPose2, float t){
	PlayerPose result = {};
	result.root = Transform::Lerp(playerPose1.root, playerPose2.root, t);
	result.head = Transform::Lerp(playerPose1.head, playerPose2.head, t);
	result.body = Transform::Lerp(playerPose1.body, playerPose2.body, t);
	result.leftArm = Transform::Lerp(playerPose1.leftArm, playerPose2.leftArm, t);
	result.rightArm = Transform::Lerp(playerPose1.rightArm, playerPose2.rightArm, t);
	return result;
}

//コンストラクタ
IPlayerState::IPlayerState(){
}

//デストラクタ
IPlayerState::~IPlayerState(){
}

//オブジェクト3dの設定
void IPlayerState::SetObject3d(Object3d* object3d){
	object3d_ = object3d;
}

//過去のポーズを設定
void IPlayerState::SettingPreviousPose(){
	prePose_.root = object3d_->GetNodeLocalTransform("Player_Root");
	prePose_.head = object3d_->GetNodeLocalTransform("Player_Root/Head");
	prePose_.body = object3d_->GetNodeLocalTransform("Player_Root/Body");
	prePose_.rightArm = object3d_->GetNodeLocalTransform("Player_Root/Arm_R");
	prePose_.leftArm = object3d_->GetNodeLocalTransform("Player_Root/Arm_L");
}

//モーション遷移の初期化
void IPlayerState::InitializeTransition(){
	//切り替え用のタイマーをリセット
	transitionTimer_ = 0.0f;

	//過去のポーズを設定
	SettingPreviousPose();
}

//モーション遷移の更新
void IPlayerState::UpdateTransition(PlayerPose targetPose){
	//切り替え用のタイマー
	transitionTimer_ += mathUtility::kDeltaTime;
	//ブレンドする割合
	float blendRate = std::clamp(transitionTimer_ / kTransitionDuration, 0.0f, 1.0f);
	//目標の位置まで補間
	currentPose_ = PlayerPose::Lerp(prePose_, targetPose, blendRate);
}