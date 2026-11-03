#pragma once
#include "BlendMode.h"
#include "ParticleData.h"
#include "RasterizerMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <list>
#include <memory>
#include <vector>

//前方宣言
class Model;
class MaterialInstance;

//パーティクルの描画に使用するデータ
struct ParticleRenderData{
	Model* model = nullptr;
	const std::list<Particle>* particles = nullptr;
	std::string pixelShaderFilePath = "";
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillMode = rasterizerMode::FillMode::kSolid;
	uint32_t numInstance = 0;
	std::vector <std::shared_ptr<MaterialInstance>> materialInstances;
	Microsoft::WRL::ComPtr<ID3D12Resource>materialResource = nullptr;
};