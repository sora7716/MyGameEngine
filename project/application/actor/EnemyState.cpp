#include "EnemyState.h"
#include "func/Physics.h"
#include "func/Rendering.h"
#include "func/Math.h"

//敵のスポーン位置のセッター
void IEnemyState::SetEnemySpawnPos(const Vector3& enemySpawnPos) {
	enemySpawnPos_ = enemySpawnPos;
}

//ターゲットの位置のセッター
void IEnemyState::SetTargetPos(const Vector3& targetPos) {
	targetPos_ = targetPos;
}

//スポーン
void EnemeyStateSpawn::Exce(GameObject& gameObject, Collider& collider) {
	gameObject.isAlive = true;
	collider.SetIsEnebled(true);
	gameObject.transformData.translate = enemySpawnPos_;
	gameObject.transformData.quaternion.y = Math::kPi;
	gameObject.acceleration.y = Physics::kGravity;
}

//待機
void EnemeyStateIdol::Exce(GameObject& gameObject, Collider& collider) {
	(void)gameObject;
	(void)collider;
}

//追従
void EnemyStateChase::Exce(GameObject& gameObject, Collider& collider) {
	//生存してなければ
	if (!gameObject.isAlive) {
		return;
	}

	//ターゲットの方向を向く
	gameObject.transformData.quaternion.y = EnemyToTarget(gameObject);

	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateMatrix(Rendering::MakeRotateQuaternion(gameObject.transformData.quaternion));
	Vector3 moveDir = { 0.0f,0.0f,1.0f };
	//カメラの向いてる方向を正にする(XとZ軸限定)
	moveDir = Math::TransformNormal(moveDir, rotMat);
	//カメラを移動させる
	gameObject.transformData.translate += moveDir.Normalize() * moveSpeed_;
}

//ターゲットの方向を向く
float EnemyStateChase::EnemyToTarget(const GameObject& gameObject) {
	//プレイヤーの向きに合わせる
	Vector3 dir = (targetPos_ - gameObject.transformData.translate).Normalize();
	float yaw = std::atan2(dir.x, dir.z);
	return yaw;
}


//
////攻撃
//void EnemeyStateAttack::Exce() {
//	enemy_->Attack();
//}