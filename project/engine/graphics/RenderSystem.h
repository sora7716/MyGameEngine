#pragma once
#include "blendMode.h"
#include <wrl.h>
#include <d3d12.h>
#include <array>

//前方宣言
class DirectXBase;
class GraphicsPipeline;
class Blend;

/// <summary>
/// 描画のシステム
/// </summary>
class RenderSystem{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RenderSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RenderSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase*directXBase);
private://メンバ関数
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	GraphicsPipeline* makeGraphicsPipeline_ = nullptr;
	//ブレンド
	Blend* blend_ = nullptr;
	BlendMode blendMode_ = BlendMode::kNone;
};

