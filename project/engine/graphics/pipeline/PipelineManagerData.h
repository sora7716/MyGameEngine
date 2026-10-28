#pragma once
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>

//パイプラインのセット
struct PipelineSet{
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
};

//パイプラインタイプ
enum class PipelineType :uint32_t{
	kObject3d,
	kParticle,
	kWireframe,
	kSkyBox,
	kSprite,
	kPiplineTypeCount
};