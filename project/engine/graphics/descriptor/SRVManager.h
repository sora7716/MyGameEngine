#pragma once
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"
#include <stdint.h>
#include <wrl.h>
#include <d3d12.h>
#include <queue>
#include <memory>

//前方宣言
class DirectXBase;
class Core;

/// <summary>
/// SRV管理
/// </summary>
class SRVManager{
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
	/// <param name="directXBase">DirectXBaseの基盤部分</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<SRVManager>Create(ConstructorKey key, DirectXBase* directXBase);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit SRVManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SRVManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXBaseの基盤部分</param>
	void Initialize(DirectXBase* directXBase);

	/// <summary>
	/// 確保
	/// </summary>
	/// <returns>インデックス</returns>
	uint32_t Allocate();

	/// <summary>
	/// 解放
	/// </summary>
	/// <param name="index">インデックス</param>
	void Free(uint32_t index);

	/// <summary>
	/// SRV生成(テクスチャ用)
	/// </summary>
	/// <param name="metadata">画面の幅などの調整</param>
	/// <param name="srvIndex">srvインデックス</param>
	/// <param name="resource">リソース</param>
	/// <param name="mipLevels">ミップレベル</param>
	void CreateSRVForTexture2D(DirectX::TexMetadata metadata, uint32_t srvIndex, ID3D12Resource* resource, UINT mipLevels);

	/// <summary>
	/// SRV生成(Structured Buffer用)
	/// </summary>
	/// <param name="srvIndex">srvインデックス</param>
	/// <param name="resource">リソース</param>
	/// <param name="numElements">要素数</param>
	/// <param name="structureByteStride"></param>
	void CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* resource, UINT numElements, UINT structureByteStride);

	/// <summary>
	/// 描画開始位置
	/// </summary>
	void PreDraw();

	/// <summary>
	/// rootDescriptorTableの設定
	/// </summary>
	/// <param name="rootParameterIndex">rootParameterのインデックス</param>
	/// <param name="srvIndex">srvインデックス</param>
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex);

	/// <summary>
	/// 最大テクスチャを超えて読み込もうとしてるかチェック
	/// </summary>
	/// <param name="kSRVTop">SRVの最初の値</param>
	/// <returns>最大テクスチャを超えて読み込もうとしてるか</returns>
	bool TextureLimitCheck(uint32_t kSRVTop);

	/// <summary>
	/// CPUデスクリプタハンドルの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>CPUデスクリプタハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// GPUデスクリプタハンドルの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>GPUデスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// デスクリプタヒープの取得
	/// </summary>
	/// <returns>デスクリプタヒープ</returns>
	ID3D12DescriptorHeap* GetDescriptorHeap()const;
private://メンバ関数
	//コピーコンストラクタ禁止
	SRVManager(const SRVManager&) = delete;
	//代入演算子の禁止
	SRVManager& operator=(const SRVManager&) = delete;
public://定数
	static inline const uint32_t kMaxSRVCount = 65536;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRV用のデスクリプタサイズ
	uint32_t descriptorSize_ = 0;
	//SRV用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descriptorHeap_ = nullptr;
	//次に使用するSRVインデックス
	uint32_t useIndex_ = 0;
	//空きリスト
	std::queue<uint32_t>freeList_;
};

