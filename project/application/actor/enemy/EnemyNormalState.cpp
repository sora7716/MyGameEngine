#include "EnemyNormalState.h"
#include <cassert>

//コンストラクタ
EnemyNormalState::EnemyNormalState(){
}

//デストラクタ
EnemyNormalState::~EnemyNormalState(){
}

//初期化
void EnemyNormalState::Initialize(Enemy* enemy){
	assert(enemy);
	enemy_ = enemy;
}

//更新
void EnemyNormalState::Update(){
}

//終了
void EnemyNormalState::Finalize(){
}
