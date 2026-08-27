#pragma once
#include "RenderingData.h"
#include "ParticleData.h"
#include <list>
#include <random>

/// <summary>
/// パーティクルの発生源
/// </summary>
class ParticleEmitter{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleEmitter();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 生存しているパーティクルの数の取得
	/// </summary>
	/// <returns>生存しているパーティクルの数</returns>
	const uint32_t GetNumInstance()const;

	/// <summary>
	/// エミッター位置の設定の設定
	/// </summary>
	/// <param name="position">エミッター位置の設定</param>
	void SetEmitterPosition(const Vector3& position);

	/// <summary>
	/// パーティクルの数の設定
	/// </summary>
	/// <param name="cont">パーティクルの数</param>
	void SetParticleCount(uint32_t cont);

	/// <summary>
	/// 発生範囲の設定
	/// </summary>
	/// <param name="range">範囲</param>
	void SetEmitRange(float range);

	/// <summary>
	/// 加速度が起こるフィールドの設定
	/// </summary>
	/// <param name="field">フィールド</param>
	void SetAccelerationField(const AccelerationField& field);

	/// <summary>
	/// パーティクルの発生感覚[秒]の設定
	/// </summary>
	/// <param name="frequency">パーティクルの発生感覚</param>
	void SetFrequency(float frequency);

	/// <summary>
	/// パーティクルを取得
	/// </summary>
	/// <returns>パーティクル</returns>
	const std::list<Particle>& GetParticles()const;
private://メンバ関数
	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <returns>新しいパーティクル</returns>
	Particle MakeNewParticle();

	/// <summary>
	/// 通常のパーティクルを生成
	/// </summary>
	/// <returns>パーティクル</returns>
	Particle MakeNormalParticle();

	/// <summary>
	/// パーティクルの発生
	/// </summary>
	/// <returns>パーティクル</returns>
	std::list<Particle>Emit();

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="point">point</param>
	/// <returns>衝突判定</returns>
	bool IsCollision(const primitiveData::AABB& aabb, const Vector3& point);
public://静的メンバ変数
	//パーティクルの数
	static const uint32_t kNumMaxInstance = 1024;
private://メンバ変数
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//パーティクルのデータ
	std::list<Particle> particles_ = {};
	//生存しているパーティクルの数
	uint32_t numInstance_ = 0;

	//発生源
	Emitter emitter_ = {
		.translate = {0.0f,0.0f,0.0f},
		.count = 1,
		.frequency = 1.0f,//発生頻度
		.frequencyTime = 0.0f,//発生頻度用の時刻,0.0fで初期化
		.range = 1.0f
	};

	//フィールドの加速度
	AccelerationField accelerationField_ = {
		.acceleration = {0.0f,0.0f,0.0f},
		.area = {{-1.0f,-1.0f,-1.0f},1.0f,1.0f,1.0f}
	};
};

