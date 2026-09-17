#include "EnemyRecoverState.h"
#include "mathUtility.h"
#include "Enemy.h"
#include "RigidBody.h"
#include "GameObject.h"
#include <cassert>

//コンストラクタ
EnemyRecoverState::EnemyRecoverState(){
}

//デストラクタ
EnemyRecoverState::~EnemyRecoverState(){
}

//初期化
void EnemyRecoverState::Initialize(Enemy* enemy){
	//敵を記録
	assert(enemy);
	enemy_ = enemy;

	//ゲームオブジェクトを取得
	gameObject_ = enemy_->GetOwner();
	//リジットボディを取得
	rigidBody_ = enemy_->GetRigidBody();
	//衝突した方向を取得
	hitDirection_ = enemy_->GetHitDirection();
	//一番最初所回転
	normalRotate_ = enemy_->GetNormalRotate();

	//起き上がるの初期化
	InitializeRecover();
}

//更新
void EnemyRecoverState::Update(){
	//起き上がるの更新
	UpdateRecover();
}

//終了
void EnemyRecoverState::Finalize(){
}

//浮き上がるときの初期化
void EnemyRecoverState::InitializeRecover(){
	//タイマーのリセット
	recoverTimer_ = 0.0f;
	//元に戻す瞬間の回転
	recoverStartRotate_ = gameObject_->GetTransform().quaternion;
}

//浮き上がるときの更新
void EnemyRecoverState::UpdateRecover(){
	//タイマーの加算
	recoverTimer_ += mathUtility::kDeltaTime;

	//係数を求める
	float t = recoverTimer_ / kRecoverDuration;

	//元に戻す
	Quaternion& rotate = gameObject_->GetTransform().quaternion;
	//補間する
	rotate = rotate.Slerp(recoverStartRotate_, normalRotate_, t);

	//時間が過ぎたら
	if (recoverTimer_ >= kRecoverDuration){
		//振る舞いのリクエストを送信
		enemy_->SetBehaviorRequest(Enemy::Behavior::kNormal);
	}
}

