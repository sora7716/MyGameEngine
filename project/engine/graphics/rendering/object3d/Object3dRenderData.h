#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//前方宣言
class Camera;

//Object3dの描画ハンドル
using Object3dRenderHandle = uint32_t;
constexpr Object3dRenderHandle kInvalidObject3dRenderHandle = UINT32_MAX;

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
	std::vector<std::vector<TransformationMatrix>>transformationData;
	std::vector<uint32_t>drawCounts = {};
};

//描画に必要なデータ
struct Object3dRenderData{
	Object3dRenderHandle renderHandle_ = kInvalidObject3dRenderHandle;
	Camera* renderCamera = nullptr;
	LODRenderData lodRenderData;
	BlendMode blendMode;
};