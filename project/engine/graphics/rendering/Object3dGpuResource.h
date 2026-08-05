#pragma once
#include "RenderData.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//LodのGPUに送る用のデータ
struct LODGpuResource{
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = nullptr;
	TransformationMatrix* wvpData = nullptr;
	uint32_t srvIndex = 0;
	uint32_t capacity = 0;
};

//Object3dのGPUに送る用のデータ
struct Object3dGpuResource{
	std::vector<LODGpuResource> lodResources;
};