#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "ParticleGpuResource.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <memory>

//前方宣言
class Camera;
class Mesh;

//Particleの描画ハンドル
using ParticleRenderHandle = uint32_t;
constexpr ParticleRenderHandle kInvalidParticleRenderHandle = UINT32_MAX;

//パーティクルの描画に使用するデータ
struct ParticleRenderData{
	Camera* renderCamera = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>materialResources;
	uint32_t srvIndex;
	std::vector<std::string> imageTexturePaths;
	ParticleForGPU* instanceData;
	BlendMode blendMode = BlendMode::kNone;
	std::vector <std::shared_ptr<Mesh>> meshes;
	uint32_t numInstance = 0;
	std::vector<std::vector<ParticleForGPU>>particleForGpuDatas;
	ParticleRenderHandle handle = kInvalidParticleRenderHandle;
};