#pragma once
#include "BlendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <string>

//前方宣言
class Camera;

//デバッグ描画で必要な描画データ
struct DebugDrawRenderData{
	Camera* renderCamera = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	Microsoft::WRL::ComPtr<ID3D12Resource>materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource;
	BlendMode blendMode = BlendMode::kNone;
	uint32_t indexCount = 0;
};