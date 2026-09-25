#include "PlayerAttackState.h"
#include "MathUtility.h"
#include "Player.h"
#include "Object3d.h"
#include <cassert>

//初期化テーブル
std::array<PlayerAttackState::AttackPhaseFunc, 2>PlayerAttackState::initializeTable = {
	&InitializeWindup,
	&InitializeSwing,
};

//更新テーブル
std::array<PlayerAttackState::AttackPhaseFunc, 2>PlayerAttackState::updateTable = {
	&UpdateWindup,
	&UpdateSwing,
};

//コンストラクタ
PlayerAttackState::PlayerAttackState(){
}

//デストラクタ
PlayerAttackState::~PlayerAttackState(){
}

//初期化
void PlayerAttackState::Enter(){

}

//更新
void PlayerAttackState::Update(){
	//リクエストされたか
	if (attackPhaseRequest_ != AttackPhase::kCount){
		//リクエストをコピー
		attackPhase_ = attackPhaseRequest_;
		//リクエストをリセット
		attackPhaseRequest_ = AttackPhase::kCount;

		//動きの初期化
		(this->*initializeTable[static_cast<uint32_t>(attackPhase_)])();
	}

	//動きの更新
	(this->*updateTable[static_cast<uint32_t>(attackPhase_)])();

	//ポーズを適応
	object3d_->SetNodeLocalTransform("Player_Root", currentPose_.root);
	object3d_->SetNodeLocalTransform("Player_Root/Arm_L", currentPose_.leftArm);
}

//解放
void PlayerAttackState::Exit(){
}

//振りかぶり状態の初期化
void PlayerAttackState::InitializeWindup(){
	//振り上げモーションのタイマーの初期化
	windupTimer_ = 0.0f;

	//過去のポーズを設定
	SettingPreviousPose();
}

//振りかぶり状態の更新
void PlayerAttackState::UpdateWindup(){
	//プレイヤーの目標ポーズ
	IPlayerState::PlayerPose targetPose = currentPose_;

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
void PlayerAttackState::InitializeSwing(){
	//タイマーリセット
	swingTimer_ = 0.0f;
	//ポーズを保存
	SettingPreviousPose();
}

//振り下ろし状態の更新
void PlayerAttackState::UpdateSwing(){
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
		player_->SetBehaviorRequest(Player::Behavior::kNormal);
		//攻撃フェーズをリセット
		attackPhaseRequest_ = AttackPhase::kWindup;
	}
}

