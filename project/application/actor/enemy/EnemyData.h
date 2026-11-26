#pragma once
#include <cstdint>

//敵のステータス
enum class EnemyStatus :uint32_t {
	kNone,
	kChill = 1 << 0,//冷えで移動が遅くなる
	kShock = 1 << 1,//一定間隔でダメージとスタン
};

//AND演算
EnemyStatus operator& (EnemyStatus status1,EnemyStatus status2);

//OR演算
EnemyStatus operator| (EnemyStatus status1,EnemyStatus status2);

//反転
EnemyStatus operator~ (EnemyStatus status);

//敵のショック状態のパラメータ
struct ShockParameter {
	//ショックが最大でどれくらい継続するか
	float maxDuration = 0.0f;
	//スタンやダメージの行う間隔
	float tickiInterval = 0.0f;
	//固定ダメージ
	float damage = 0.0f;
};

//敵のショックのステート
struct ShockState {
	//ショックの経過時間
	float elapsedTime_ = 0.0f;
	//スタンやダメージの起こる経過時間
	float tickTimer_ = 0.0f;
	//今スタン中か
	bool isStunned = false;
};