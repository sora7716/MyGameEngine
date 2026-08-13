#pragma once
#include "RenderData.h"
#include <wrl.h>
#include <d3d12.h>

//パーティクルの情報をGPUに送るための構造体
struct ParticleForGPU{
	Matrix4x4 world = Matrix4x4::Identity4x4();
	Vector4 color = Vector4::MakeWhiteColor();
};

//パーティクルのGPUリソース
struct ParticleGpuResource{
	Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource = nullptr;
	ParticleForGPU* instanceData = nullptr;
	uint32_t srvIndex = 0;
	uint32_t capacity = 0;
};