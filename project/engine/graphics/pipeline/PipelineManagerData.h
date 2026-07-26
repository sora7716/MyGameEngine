#pragma once
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>

//パイプラインのセット
struct PipelineSet{
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature = nullptr;
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates = { nullptr };
};

//パイプラインタイプ
enum class PiplineType :uint32_t{
	kObject3d,
	kSprite,
	kParticle,
	kSkyBox,
	kPiplineTypeCount
};