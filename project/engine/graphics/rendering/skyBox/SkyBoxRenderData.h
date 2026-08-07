#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <string>

//前方宣言
class Camera;

/// <summary>
/// スカイボックスの描画データ
/// </summary>
struct SkyBoxRenderData{
	bool isActive;
	Camera* renderCamera = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	Microsoft::WRL::ComPtr<ID3D12Resource>materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource;
	std::string imageFileName = "";
	BlendMode blendMode = BlendMode::kNone;
	uint32_t indexCount = 0;
};