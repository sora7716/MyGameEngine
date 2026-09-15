#include "EnemyRootState.h"
#include <cassert>

//コンストラクタ
EnemyRootState::EnemyRootState(){
}

//デストラクタ
EnemyRootState::~EnemyRootState(){
}

//初期化
void EnemyRootState::Initialize(Enemy* enemy){
	assert(enemy);
	enemy_ = enemy;
}

//更新
void EnemyRootState::Update(){
}

//終了
void EnemyRootState::Finalize(){
}
