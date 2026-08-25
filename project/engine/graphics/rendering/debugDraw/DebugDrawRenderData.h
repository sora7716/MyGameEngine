#pragma once
#include "BlendMode.h"
#include "Vector4.h"
#include "Vector2.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>

//頂点データ
struct DebugDrawVertexData{
	Vector4 position;
	Vector2 texcoord;
};

//デバッグ描画で必要な描画データ
struct DebugDrawRenderData{
	std::vector<DebugDrawVertexData>vertices_ = {};
	std::vector<uint32_t>indices_ = {};
	Microsoft::WRL::ComPtr<ID3D12Resource>materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource;
	BlendMode blendMode = BlendMode::kNone;
};