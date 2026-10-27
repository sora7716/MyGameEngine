#pragma once
#include "BlendMode.h"
#include "ParticleData.h"
#include <list>

//前方宣言
class Model;
class MaterialInstance;

//パーティクルの描画に使用するデータ
struct ParticleRenderData{
	Model* model = nullptr;
	MaterialInstance* materialInstance = nullptr;
	const std::list<Particle>* particles = nullptr;
	BlendMode blendMode = BlendMode::kNone;
	uint32_t numInstance = 0;
};