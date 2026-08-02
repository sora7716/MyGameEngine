#pragma once
#include "RenderData.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//前方宣言
class Camera;
class Mesh;

//モデルの描画に必要なデータ
struct ModelRenderData{
	Microsoft::WRL::ComPtr<ID3D12Resource>rimLightResource = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>materialResources;
	ModelData modelData = {};
	std::vector<Mesh*>meshes;
};

//LODの描画に必要なデータ
struct LODRenderData{
	std::vector<ModelRenderData>modelRendererData;
	std::vector<uint32_t>srvIndices = {};
	std::vector<uint32_t>drawCounts = {};

	/// <summary>
	/// サイズがあっている確認
	/// </summary>
	/// <returns>サイズがあっているか</returns>
	bool IsLodCountValid()const;
};

//描画に必要なデータ
struct Object3dRenderData{
	Camera* renderCamera = nullptr;

	LODRenderData lodRenderData;
};