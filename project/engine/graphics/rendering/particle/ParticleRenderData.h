#pragma once
#include "BlendMode.h"
#include "ParticleData.h"
#include "RasterizerMode.h"
#include <list>

//前方宣言
class Model;
class MaterialInstance;

//パーティクルの描画に使用するデータ
struct ParticleRenderData{
	Model* model = nullptr;
	MaterialInstance* materialInstance = nullptr;
	const std::list<Particle>* particles = nullptr;
	std::string pixelShaderFilePath = "";
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillMode = rasterizerMode::FillMode::kSolid;
	uint32_t numInstance = 0;
};