#pragma once
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>
#include <memory>

//前方宣言
class DirectXBase;
class GraphicsPipeline;
class Blend;

//パイプラインタイプ
enum class PiplineType :uint32_t{
	kObject3d,
	kSprite,
	kParticle,
	kSkyBox,
	kPiplineTypeCount
};

//パイプラインのセット
struct PipelineSet{
	ComPtr<ID3D12RootSignature>rootSignature = nullptr;
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates = { nullptr };
	BlendMode blendMode_ = BlendMode::kNone;
};

/// <summary>
/// パイプラインの管理
/// </summary>
class PipelineManager{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PipelineManager();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PipelineManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
private://メンバ変数
	//ブレンド
	std::unique_ptr<Blend> blend_ = nullptr;
	//グラフィックスパイプライン
	std::unique_ptr<GraphicsPipeline> makeGraphicsPipeline_ = nullptr;
	//各オブジェクトごとにパイプラインタイプ
	std::array<PipelineSet, static_cast<uint32_t>(PiplineType::kPiplineTypeCount)> pipelineSets_ = {};
};

