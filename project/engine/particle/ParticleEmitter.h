#pragma once
#include <list>
#include <random>
#include "RenderingData.h"
#include "Vector4.h"
#include "PrimitiveData.h"

//前方宣言
class ParticleCommon;
class Camera;
class Model;

//パーティクルの情報をGPUに送るための構造体
struct ParticleForGPU {
	Matrix4x4 WVP = Matrix4x4::Identity4x4();
	Matrix4x4 World = Matrix4x4::Identity4x4();
	Vector4 color = Vector4::MakeWhiteColor();
};

//パーティクル単体のデータ
struct Particle {
	Transform transform = {};//SRVの情報
	Vector3 velocity = {};//方向
	Vector4 color = Vector4::MakeWhiteColor();//色
	float lifeTime = 0.0f;//生存時間
	float currentTime = 0.0f;//発生してからの
};

//発生源
struct Emitter {
	Vector3 translate = {};//エミッターのTransform
	uint32_t count = 0;//発生数
	float frequency = 0.0f;//発生頻度
	float frequencyTime = 0.0f;//頻度用時刻
	float range = 0.0f;//発生範囲
};

//フィールドの加速度
struct AccelerationField {
	Vector3 acceleration = {};//加速度
	PrimitiveData::AABB area = {};//範囲
};

/// <summary>
/// パーティクルの発生源
/// </summary>
class ParticleEmitter {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleEmitter() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="particleCommon">パーティクルの共通部分</param>
	/// <param name="renderCamera">描画用カメラ</param>
	/// <param name="model">モデル</param>
	void Initialize(ParticleCommon* particleCommon, Camera* renderCamera, Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="instancingData">インスタンシングデータ</param>
	void Update(ParticleForGPU* instancingData);

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// モデルの取得
	/// </summary>
	/// <returns></returns>
	Model* GetModel()const;

	/// <summary>
	/// 生存しているパーティクルの数の取得
	/// </summary>
	/// <returns></returns>
	const uint32_t GetNumInstance()const;

	/// <summary>
	/// カメラの設定
	/// </summary>
	/// <param name="camera"></param>
	void SetGameCamera(Camera* gameCamera);

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
	/// ワールドトランスフォームの更新
	/// </summary>
	/// <param name="numInstance">インスタンス数</param>
	/// <param name="iterator"イテレータ></param>
	/// <param name="instancingData">インスタンシングデータ</param>
	void UpdateWorldTransform(uint32_t numInstance, auto iterator, ParticleForGPU* instancingData);

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
	bool IsCollision(const PrimitiveData::AABB& aabb, const Vector3& point);
public://静的メンバ変数
	//パーティクルの数
	static const uint32_t kNumMaxInstance = 1024;
private://メンバ変数
	//パーティクルの共通部分
	ParticleCommon* particleCommon_ = nullptr;
	//カメラ
	Camera* gameCamera_ = nullptr;
	//描画用のカメラ
	Camera* renderCamera_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	//ランダムエンジン
	std::mt19937 randomEngine_;
	//パーティクルのデータ
	std::list<Particle> particles_ = {};
	//ワールドマトリックス
	Matrix4x4 worldMatrix_ = {};
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

