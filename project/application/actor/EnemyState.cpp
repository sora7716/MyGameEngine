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

//コライダーのセッター
void IEnemyState::SetColliderPtr(Collider* colliderPtr) {
	colliderPtr_ = colliderPtr;
}

//基準となるyawのセッター
void IEnemyState::SetBaseYaw(float baseYaw) {
	baseYaw_ = baseYaw;
}

//前方に動かす
Vector3 IEnemyState::MoveForward(const Quaternion& quaternion, float speed) {
	//カメラの角度をもとに回転行列を求める
	Matrix4x4 rotMat = Rendering::MakeRotateMatrix(Rendering::MakeRotateQuaternion(quaternion));
	Vector3 moveDir = { 0.0f,0.0f,1.0f };
	//カメラの向いてる方向を正にする(XとZ軸限定)
	moveDir = Math::TransformNormal(moveDir, rotMat);
	//移動させる速度を返す
	return moveDir.Normalize() * speed;
}

//スポーン
void EnemeyStateSpawn::Exce(GameObject& gameObject) {
	gameObject.isAlive = true;
	colliderPtr_->SetIsEnebled(true);
	gameObject.transformData.translate = enemySpawnPos_;
	gameObject.transformData.quaternion.y = Math::kPi;
	gameObject.acceleration.y = Physics::kGravity;
}

//待機
void EnemeyStateIdol::Exce(GameObject& gameObject) {
	rotateTime_ += 0.5f * Math::kDeltaTime;

	float amplitude = Math::kPi / 2.0f;
	float angle = baseYaw_ + std::sin(rotateTime_) * amplitude;

	gameObject.transformData.quaternion.y = angle;
}

//追従
void EnemyStateChase::Exce(GameObject& gameObject) {
	//生存してなければ
	if (!gameObject.isAlive) {
		return;
	}

	//ターゲットの方向を向く
	gameObject.transformData.quaternion = EnemyToTarget(gameObject);

	//移動させる
	gameObject.transformData.translate += MoveForward(gameObject.transformData.quaternion, moveSpeed_);
}

//ターゲットの方向を向く
Quaternion EnemyStateChase::EnemyToTarget(const GameObject& gameObject) {
	//プレイヤーの向きに合わせる
	Vector3 dir = (targetPos_ - gameObject.transformData.translate).Normalize();
	float yaw = std::atan2(dir.x, dir.z);
	Quaternion result = gameObject.transformData.quaternion;
	result.y = yaw;
	return result;
}

//パトロール
void EnemyStatePatrol::Exce(GameObject& gameObject) {
	//移動させる
	gameObject.transformData.translate += MoveForward(gameObject.transformData.quaternion, moveSpeed_);
}
