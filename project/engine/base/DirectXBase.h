#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
#include <vector>
#include <array>
#include "Vector4.h"
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"

//前方宣言
class WinApi;
class Core;

/// <summary>
/// DirectXコモン
/// </summary>
class DirectXBase{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="winApi">ウィンドウズAPI</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<DirectXBase>Create(ConstructorKey key, WinApi* winApi);

	/// <summary>
	/// デスクリプターCPUハンドルの取得
	/// </summary>
	/// <param name="descriptorHeap">デスクリプターヒープ</param>
	/// <param name="descriptorSize">デスクリプターサイズ</param>
	/// <param name="index">インデックス</param>
	/// <returns>デスクリプターCPUハンドル</returns>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	/// <summary>
	/// デスクリプターGPUハンドルの取得
	/// </summary>
	/// <param name="descriptorHeap">デスクリプターヒープ</param>
	/// <param name="descriptorSize">デスクリプターサイズ</param>
	/// <param name="index">インデックス</param>
	/// <returns>デスクリプターGPUハンドル</returns>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
private://定数
	//スワップチェインのバックバッファの数
	static inline const uint32_t kBackBufferCount = 2;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	DirectXBase(ConstructorKey);

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
	/// コマンド関連の生成
	/// </summary>
	void CreateCommands();

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
	/// <param name="rtvHandle">rtvのハンドル</param>
	/// <param name="dsvHandle">dsvのハンドル</param>
	void PreDraw(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	/// <summary>
	/// 描画終了位置
	/// </summary>
	void PostDraw();

	/// <summary>
	/// DescriptorHeapの作成
	/// </summary>
	/// <param name="heapType">ヒープタイプ</param>
	/// <param name="numDescriptors">デスクリプターの番号</param>
	/// <param name="shaderVisible">シェーダを使うか</param>
	/// <returns>デスクリプターヒープ</returns>
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

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
	/// <param name="metaDada">メタデータ</param>
	/// <returns>テクスチャリソース</returns>
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metaDada);

	/// <summary>
	/// TextureResourceにデータを転送する 
	/// </summary>
	/// <param name="texture">テクスチャ</param>
	/// <param name="mipImages">ミップマップ</param>
	/// <returns>TextureResourceにデータを転送する</returns>
	[[nodiscard]]//属性という機能(戻り値を破棄してはならない)むやみやたらとつけてはいけない
	ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, D3D12_RESOURCE_STATES& inOutState, const DirectX::ScratchImage& mipImages);

	/// <summary>
	/// 深度バッファリソースの設定
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice()const;

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList()const;

	/// <summary>
	/// デプスステンシルテクスチャの取得
	/// </summary>
	/// <returns>デプスステンシルテクスチャ</returns>
	ID3D12Resource* GetDepthStencilTexture()const;

	/// <summary>
	/// スワップチェーンのリソースのサイズの取得
	/// </summary>
	/// <returns>スワップチェーンのリソースのサイズ</returns>
	uint32_t GetSwapChainResourceSize()const;

	/// <summary>
	/// スワップチェーンのリソースの取得
	/// </summary>
	/// <returns>スワップチェーンのリソース</returns>
	const std::array<ComPtr<ID3D12Resource>, kBackBufferCount>& GetSwapChainResources()const;

	/// <summary>
	/// バックバッファの検索キーの取得
	/// </summary>
	/// <returns>バックバッファの検索キー</returns>
	uint32_t GetBackBufferIndex()const;
private://メンバ関数
	//コピーコンストラクタ禁止
	DirectXBase(const DirectXBase&) = delete;
	//代入演算子を禁止
	const DirectXBase& operator=(const DirectXBase&) = delete;

	/// <summary>
	/// IDXIファクトリーの生成
	/// </summary>
	/// <returns>IDXIファクトリー</returns>
	ComPtr<IDXGIFactory7> CreateIDXGIFactory();

	/// <summary>
	/// 使用するアダプタを決定
	/// </summary>
	/// <returns>使用するアダプタ</returns>
	ComPtr<IDXGIAdapter4> DecideUseAdapter();

	/// <summary>
	/// D3D12デバイスの生成
	/// </summary>
	/// <returns>D3D12デバイス</returns>
	ComPtr<ID3D12Device> CreateD3D12Device();

	/// <summary>
	/// コマンドキューの生成
	/// </summary>
	/// <returns>コマンドキュー</returns>
	ComPtr<ID3D12CommandQueue> CreateCommandQueue();

	/// <summary>
	/// コマンドアローケータの生成 
	/// </summary>
	/// <returns>コマンドアローケータ</returns>
	ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();

	/// <summary>
	/// コマンドリストの生成
	/// </summary>
	/// <returns>コマンドリスト</returns>
	ComPtr<ID3D12GraphicsCommandList> CreateCommandList();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <param name="windowWidth">画面の横幅</param>
	/// <param name="windowHeight">画面の縦幅</param>
	/// <param name="bufferSize">バッファサイズ</param>
	/// <returns>スワップチェーン</returns>
	ComPtr<IDXGISwapChain4> CreateSwapChain(int32_t windowWidth, int32_t windowHeight, uint32_t bufferSize);

	/// <summary>
	/// SwapChainからResourceを引っ張ってくる
	/// </summary>
	/// <param name="swapChain">スワップチェイン</param>
	/// <param name="num">何番目か</param>
	/// <returns>リソース</returns>
	ComPtr<ID3D12Resource> BringResourcesFromSwapChain(IDXGISwapChain4* swapChain, UINT num);

	/// <summary>
	/// Fenceを作成する
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12Fence> CreateFence();

	/// <summary>
	/// デバックレイヤー
	/// </summary>
	void DebugLayer();

	/// <summary>
	/// 実行を停止する(エラー・警告の場合)
	/// </summary>
	void StopExecution();
private://メンバ変数
	//WindowAPI
	WinApi* winApi_ = nullptr;
	//デバックコントローラー
	ComPtr<ID3D12Debug1> debugController_ = nullptr;
	//IDXIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	//使用するアダプタ
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	//デバイス
	ComPtr<ID3D12Device> device_ = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	//コマンドアローケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//ゲーム画面用のスワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = { nullptr };
	//スワップチェーンからリソースを引っ張ってくる
	std::array<ComPtr<ID3D12Resource>, kBackBufferCount> swapChainResources_ = { nullptr };
	//深度バッファ
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	//Fence
	ComPtr<ID3D12Fence> fence_ = nullptr;
	//FenceEvent	
	HANDLE fenceEvent_ = 0;
	//DXCユーティリティ
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	//DXCコンパイラ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	//デフォルトインクルードハンドラ
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	//FenceValue
	uint64_t fenceValue_ = 0;
	//ビューポート
	D3D12_VIEWPORT viewport_{};
	//シーザー矩形
	D3D12_RECT scissorRect_{};
};