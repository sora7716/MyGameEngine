#include "EnemyState.h"
#include "Enemy.h"

//敵のセッター
void IEnemyState::SetEnemy(Enemy* enemy) {
	enemy_ = enemy;
}

//スポーン
void EnemeyStateSpawn::Exce() {
	enemy_->Spawn();
}

//待機
void EnemeyStateIdol::Exce() {
	enemy_->Idol();
}

//追従
void EnemyStateChase::Exce() {
	enemy_->Chase();
}
//
////攻撃
//void EnemeyStateAttack::Exce() {
//	enemy_->Attack();
//}
