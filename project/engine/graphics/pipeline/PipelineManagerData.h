#pragma once
#include "blendMode.h"
#include "RasterizerMode.h"
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

//パイプラインの検索キー
struct PipelineKey{
	std::string pixelShaderFilePath = "";
	PipelineType pipelineType = PipelineType::kObject3d;
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillMode = rasterizerMode::FillMode::kSolid;

	//パイプライン同じか確認
	bool operator==(const PipelineKey& pipelineKey)const;

	/// <summary>
	/// ハッシュキーを作成
	/// </summary>
	/// <returns>ハッシュキー</returns>
	size_t MakeHashKey()const;
};