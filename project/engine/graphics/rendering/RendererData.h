#pragma once
#include "RenderData.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//前方宣言
class Camera;
class Mesh;

//メッシュの描画に必要なデータ
struct MeshRenderData{
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	MeshData meshData = {};
};


//モデルの描画に必要なデータ
struct ModelRenderData{
	Microsoft::WRL::ComPtr<ID3D12Resource>rimLightResource = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>materialResources;
	ModelData modelData = {};
	std::vector<MeshRenderData>meshRenderDatas;
};

//LODの描画に必要なデータ
struct LODRenderData{
	std::vector<ModelRenderData>modelRendererDatas;
	std::vector<uint32_t>wvpSrvIndices = {};
	std::vector<uint32_t>drawCounts = {};
};

//描画に必要なデータ
struct Object3dRenderData{
	Camera* renderCamera = nullptr;
	LODRenderData lodRenderData;
};

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