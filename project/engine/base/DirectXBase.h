#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <vector>
#include <array>
#include "engine/math/Vector4.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

//前方宣言
class WinApi;
class Core;

/// <summary>
/// DirectXコモン
/// </summary>
class DirectXBase {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~DirectXBase();
	
	/// <summary>
	/// DirectX12の初期化
	/// </summary>
	/// <param name="winApi">ウィンドウズアプリケーション</param>
	void Initialize(WinApi* winApi);

	/// <summary>
	/// デバイスの初期化
	/// </summary>
	void InitializeDevice();

	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// 各種デスクリプターヒープの生成
	/// </summary>
	void CreateDescriptorHeap();

	/// <summary>
	/// RTVの初期化
	/// </summary>
	void InitializeRTV();

	/// <summary>
	/// フェンスの初期化
	/// </summary>
	void InitializeFence();

	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void InitializeViewport();

	/// <summary>
	/// シザリング矩形の初期化
	/// </summary>
	void InitializeScissorRect();

	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	void CreateDXCCompiler();

	/// <summary>
	/// 描画開始位置
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画終了位置
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 深度バッファリソースの設定
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> MakeDepthStencilTextureResource(int32_t width, int32_t height);

	/// <summary>
	/// DescriptorHeapの作成
	/// </summary>
	/// <param name="heapType">ヒープタイプ</param>
	/// <param name="numDescriptors">デスクリプターの番号</param>
	/// <param name="shaderVisible">シェーダを使うか</param>
	/// <returns>デスクリプターヒープ</returns>
	ComPtr<ID3D12DescriptorHeap> MakeDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool	shaderVisible);

	/// <summary>
	/// シェーダーのコンパイラ
	/// </summary>
	/// <param name="filePath">CompilerするShaderファイルへのパス</param>
	/// <param name="profile">Compilerに使用するProfile</param>
	/// <returns>コンパイラシェーダー</returns>
	ComPtr<IDxcBlob> CompilerShader(const std::wstring& filePath, const wchar_t* profile);

	/// <summary>
	/// バッファリソースの生成
	/// </summary>
	/// <param name="sizeInBytes">サイズ</param>
	/// <returns>バッファリソース</returns>
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	/// <summary>
	/// テクスチャリソースの生成
	/// </summary>
	/// <param name="metadada">メタデータ</param>
	/// <returns>テクスチャリソース</returns>
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadada);

	/// <summary>
	/// TextureResourceにデータを転送する 
	/// </summary>
	/// <param name="texture">テクスチャ</param>
	/// <param name="mipImages">ミップマップ</param>
	/// <returns>TextureResourceにデータを転送する</returns>
	[[nodiscard]]//属性という機能(戻り値を破棄してはならない)むやみやたらとつけてはいけない
	ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, D3D12_RESOURCE_STATES& inOutState, const DirectX::ScratchImage& mipImages);

	/// <summary>
	/// RTVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index">検索番号</param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// RTVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index">検索番号</param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// DSVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index">検索番号</param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// DSVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index">検索番号</param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// デバイスのゲッター
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice()const;

	/// <summary>
	/// コマンドリストのゲッター
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList()const;

	/// <summary>
	/// スワップチェーンのリソース数のゲッター
	/// </summary>
	/// <returns>スワップチェーンのリソース数</returns>
	size_t GetSwapChainResourceNum()const;

	/// <summary>
	/// デプスステンシルテクスチャの取得
	/// </summary>
	/// <returns>デプスステンシルテクスチャ</returns>
	ID3D12Resource* GetDepthStencilTexture()const;
public://PressKey
	class ConstructorKey {
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PressKeyを受け取る</param>
	explicit DirectXBase(ConstructorKey);
public://静的メンバ関数
	/// <summary>
	/// デスクリプターCPUハンドルのゲッター
	/// </summary>
	/// <param name="descriptorHeap">デスクリプターヒープ</param>
	/// <param name="descriptorSize">デスクリプターサイズ</param>
	/// <param name="index">インデックス</param>
	/// <returns>デスクリプターCPUハンドル</returns>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// デスクリプターGPUハンドルのゲッター
	/// </summary>
	/// <param name="descriptorHeap">デスクリプターヒープ</param>
	/// <param name="descriptorSize">デスクリプターサイズ</param>
	/// <param name="index">インデックス</param>
	/// <returns>デスクリプターGPUハンドル</returns>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

private://メンバ関数
	//コピーコンストラクタ禁止
	DirectXBase(const DirectXBase&) = delete;
	//代入演算子を禁止
	const DirectXBase operator=(const DirectXBase&) = delete;
	/// <summary>
	/// IDXIファクトリーの生成
	/// </summary>
	/// <returns>IDXIファクトリー</returns>
	ComPtr<IDXGIFactory7> MakeIDXGIFactory();

	/// <summary>
	/// 使用するアダプタを決定
	/// </summary>
	/// <returns>使用するアダプタ</returns>
	ComPtr<IDXGIAdapter4> DecideUseAdapter();

	/// <summary>
	/// D3D12デバイスの生成
	/// </summary>
	/// <returns>D3D12デバイス</returns>
	ComPtr<ID3D12Device> MakeD3D12Device();

	/// <summary>
	/// コマンドキューの生成
	/// </summary>
	/// <returns>コマンドキュー</returns>
	ComPtr<ID3D12CommandQueue> MakeCommandQueue();

	/// <summary>
	/// コマンドアローケータの生成 
	/// </summary>
	/// <returns>コマンドアローケータ</returns>
	ComPtr<ID3D12CommandAllocator> MakeCommandAllocator();

	/// <summary>
	/// コマンドリストの生成
	/// </summary>
	/// <returns>コマンドリスト</returns>
	ComPtr<ID3D12GraphicsCommandList> MakeCommandList();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <returns>スワップチェーン</returns>
	ComPtr<IDXGISwapChain4> MakeSwapChain();

	/// <summary>
	/// SwapChainからResourceを引っ張ってくる
	/// </summary>
	/// <param name="num">何番目か</param>
	/// <returns>リソース</returns>
	ComPtr<ID3D12Resource> BringResourcesFromSwapChain(UINT num);

	/// <summary>
	/// Fenceを作成する
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Fence> MakeFence();

	/// <summary>
	/// デバックレイヤー
	/// </summary>
	void DebugLayer();

	/// <summary>
	/// 実行を停止する(エラー・警告の場合)
	/// </summary>
	void StopExecution();
private://メンバ変数
	D3D12_RESOURCE_BARRIER barrier_{};//TransitionBarrierの設定
	WinApi* winApi_ = nullptr;//ウィンドウズAPI
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;//IDXIファクトリー
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;//使用するアダプタ
	ComPtr<ID3D12Device> device_ = nullptr;//デバイス
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;//コマンドキュー
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;//コマンドアローケータ
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;//コマンドリスト
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_ = {};
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};//rtvDesc
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};//DSVハンドル
	//深度バッファ
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	//DescriptorSize
	uint32_t descriptorSizeRTV_;
	std::array<ComPtr<ID3D12Resource>, 2> swapChainResources_ = { nullptr };//スワップチェーンからリソースを引っ張ってくる
	ComPtr<ID3D12Debug1> debugController_ = nullptr;//デバックコントローラー
	ComPtr<ID3D12Fence> fence_ = nullptr;//Fence
	uint64_t fenceValue_ = 0;//FenceValue
	HANDLE fenceEvent_ = 0;//FenceEvent	
	D3D12_VIEWPORT viewport_{};//ビューポート
	D3D12_RECT scissorRect_{};//シーザー矩形
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;//DXCユーティリティ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;//DXCコンパイラ
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;//デフォルトインクルードハンドラ
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;//DSV
	uint32_t descriptorSizeDSV_ = 0;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles_ = {};//RTVを2つ作るのでディスクリプタを2つ用意
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;//スワップチェーン
};