#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <stdint.h>
#include <queue>
#include <memory>

//前方宣言
class DirectXBase;
class Core;

/// <summary>
/// DSVの管理
/// </summary>
class DSVManager{
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
	static std::unique_ptr<DSVManager>Create(ConstructorKey key, DirectXBase* directXBase);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit DSVManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DSVManager();

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
	/// DSVの生成
	/// </summary>
	/// <param name="resource">リソース</param>
	/// <param name="dsvIndex">dsvの検索キー</param>
	/// <param name="format">フォーマット</param>
	void CreateDSV(ID3D12Resource* resource, uint32_t dsvIndex, DXGI_FORMAT format);

	/// <summary>
	/// CPUデスクリプタハンドルの取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>CPUデスクリプタハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
private://メンバ関数
	//コピーコンストラクタ禁止
	DSVManager(const DSVManager&) = delete;
	//代入演算子の禁止
	DSVManager& operator=(const DSVManager&) = delete;
public://定数
	static inline const uint32_t kMaxDSVCount = 3;
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

