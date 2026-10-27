#pragma once
#include "Context.h"
#include <memory>
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

/// <summary>
/// レンダーテクスチャ
/// </summary>
class RenderTexture{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ変数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="context">レンダーテクスチャで必要なもの</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<RenderTexture>Create(const RenderTextureContext& context, uint32_t width, uint32_t height);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RenderTexture();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RenderTexture();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="context">レンダーテクスチャで必要なもの</param>
	void Initialize(const RenderTextureContext& context, uint32_t width, uint32_t height);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画開始
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画終了
	/// </summary>
	void PostDraw();

	/// <summary>
	/// GPUデスクリプタハンドルの取得
	/// </summary>
	/// <returns>GPUデスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle()const;
private://メンバ関数
	/// <summary>
	/// リソースの生成
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateResource();

	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void InitializeViewport();

	/// <summary>
	/// シザリング矩形の初期化
	/// </summary>
	void InitializeScissorRect();
private://メンバ変数
	//レンダーテクスチャで必要なもの
	RenderTextureContext context_{};
	//横幅
	uint32_t width_ = 0;
	//縦幅
	uint32_t height_ = 0;
	//カラーリソース
	ComPtr<ID3D12Resource>colorResource_ = nullptr;
	//深度バッファ
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	//SRV検索キー
	uint32_t srvIndex_ = 0;
	//RTVの検索キー
	uint32_t rtvIndex_ = 0;
	//DSVの検索キー
	uint32_t dsvIndex_ = 0;
	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シーザー矩形
	D3D12_RECT scissorRect_{};
};

