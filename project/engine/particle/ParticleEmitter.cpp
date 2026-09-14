#define NOMINMAX
#include "ParticleEmitter.h"
#include "MathUtility.h"
using namespace primitiveData;

//コンストラクタ
ParticleEmitter::ParticleEmitter(){
}

//デストラクタ
ParticleEmitter::~ParticleEmitter(){
}

//初期化
void ParticleEmitter::Initialize(){
	//乱数エンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator());
}

//更新
void ParticleEmitter::Update(){
	//生存しているパーティクルの数を0に初期化
	numInstance_ = 0;

	//更新処理
	for (auto it = particles_.begin(); it != particles_.end();){
		if (numInstance_ < kNumMaxInstance){
			if ((*it).lifeTime <= (*it).currentTime){
				it = particles_.erase(it); //生存期間を過ぎたらパーティクルをlistから削除
				continue;//削除したので次のループへ
			}
			//移動
			(*it).transform.translate += (*it).velocity * mathUtility::kDeltaTime;
			//経過時間を足す
			(*it).currentTime += mathUtility::kDeltaTime;
			float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);
			(*it).color.w = alpha;

			//表示するかチェック
			if (it->isEnabled){
				numInstance_++;
			}

		}
		//次のイテレータに進める
		it++;
	}

	//衝突判定
	for (auto it = particles_.begin(); it != particles_.end();){
		//Field内のParticleには加速度を適用する
		if (IsCollision(accelerationField_.area, (*it).transform.translate)){
			(*it).velocity += accelerationField_.acceleration * mathUtility::kDeltaTime;
		}
		it++;
	}

	//Emitterの更新
	emitter_.frequencyTime += mathUtility::kDeltaTime;
	if (emitter_.frequency <= emitter_.frequencyTime){
		particles_.splice(particles_.end(), Emit());
		emitter_.frequencyTime -= emitter_.frequency;//余計に過ぎた時間も加味して頻度を計算する
	}
}

//生存しているパーティクルの数の取得
const uint32_t ParticleEmitter::GetNumInstance() const{
	return numInstance_;
}

//エミッター位置の設定
void ParticleEmitter::SetEmitterPosition(const Vector3& position){
	emitter_.translate = position;
}

//パーティクルの数の設定
void ParticleEmitter::SetParticleCount(uint32_t count){
	emitter_.count = count;
}

//発生範囲の設定
void ParticleEmitter::SetEmitRange(float range){
	emitter_.range = range;
}

//加速度が起こるフィールドの設定
void ParticleEmitter::SetAccelerationField(const AccelerationField& field){
	accelerationField_ = field;
}

//パーティクルの発生感覚の設定
void ParticleEmitter::SetFrequency(float frequency){
	emitter_.frequency = frequency;
}

//パーティクルの取得
const std::list<Particle>& ParticleEmitter::GetParticles()const{
	return particles_;
}

//パーティクルの生成
Particle ParticleEmitter::MakeNewParticle(){
	return MakeNormalParticle();
}

//通常のパーティクルを生成
Particle ParticleEmitter::MakeNormalParticle(){
	////パーティクルの初期化
	//Particle particle;

	////拡縮
	//particle.transform.scale = { 1.0f, 1.0f, 1.0f };

	////位置の値をemitRange_の範囲でランダムに設定
	//std::uniform_real_distribution<float>distributionPosition(-emitter_.range, emitter_.range);
	////位置
	//Vector3 randomTranslate = { distributionPosition(randomEngine_), distributionPosition(randomEngine_), distributionPosition(randomEngine_) };
	////パーティクルの位置を発生源を中心に設定
	//particle.transform.translate = emitter_.transform.translate + randomTranslate;

	////位置と速度を[-1.0f,1.0f]でランダムに設定
	//std::uniform_real_distribution<float>distributionVelocity(-1.0f, 1.0f);
	////移動する速度
	//particle.velocity = { distributionVelocity(randomEngine_), distributionVelocity(randomEngine_), distributionVelocity(randomEngine_) };

	////色の値を[0.0f,1.0f]でランダムに設定
	//std::uniform_real_distribution<float>distColor(0.0f, 1.0f);
	////色
	//particle.color = { distColor(randomEngine_), distColor(randomEngine_), distColor(randomEngine_),1.0f };

	////生存時間
	//std::uniform_real_distribution<float>distTime(1.0f, 3.0f);
	//particle.lifeTime = distTime(randomEngine_);
	//particle.currentTime = 0.0f;

	//パーティクルの初期化
	Particle particle;
	std::uniform_real_distribution<float>distScale(0.4f, 1.5f);
	particle.transform.scale = { 0.05f,distScale(randomEngine_),1.0f };
	std::uniform_real_distribution<float>distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
	particle.transform.SetEulerAngle({ 0.0f,0.0f,distRotate(randomEngine_) });
	particle.transform.translate = emitter_.translate;
	particle.velocity = { 0.0f,0.0f,0.0f };
	particle.color = Vector4::GetWhiteColor();
	particle.lifeTime = 1.0f;//1秒で消える
	particle.currentTime = 0;

	return particle;
}

//パーティクルの発生
std::list<Particle> ParticleEmitter::Emit(){
	std::list<Particle>particles;
	for (uint32_t i = 0; i < emitter_.count; i++){
		particles.push_back(MakeNewParticle());
	}
	return particles;
}

//衝突判定
bool ParticleEmitter::IsCollision(const AABB& aabb, const Vector3& point){
	AABB temp = aabb;
	Vector3 tNear;
	Vector3 tFar;

	temp.min.x = (aabb.min.x - point.x);
	temp.max.x = (aabb.max.x - point.x);

	temp.min.y = (aabb.min.y - point.y);
	temp.max.y = (aabb.max.y - point.y);

	temp.min.z = (aabb.min.z - point.z);
	temp.max.z = (aabb.max.z - point.z);

	tNear.x = std::min(temp.min.x, temp.max.x);
	tFar.x = std::max(temp.min.x, temp.max.x);

	tNear.y = std::min(temp.min.y, temp.max.y);
	tFar.y = std::max(temp.min.y, temp.max.y);

	tNear.z = std::min(temp.min.z, temp.max.z);
	tFar.z = std::max(temp.min.z, temp.max.z);

	float tMin = std::max(tNear.x, std::max(tNear.z, tNear.y));
	float tMax = std::min(tFar.x, std::min(tFar.z, tFar.y));
	bool isCollision = false;
	if (tMin <= tMax){
		if (tMin * tMax < 0.0f){
			isCollision = true;
		}
		if (0.0f <= tMin && tMin <= 1.0f || 0.0f <= tMax && tMax <= 1.0f){
			isCollision = true;
		}
	} else{
		isCollision = false;
	}
	return isCollision;
}