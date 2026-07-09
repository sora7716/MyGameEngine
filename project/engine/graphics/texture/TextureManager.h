#pragma once
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"
#include <string>
#include <unordered_map>

//前方宣言
class DirectXBase;
class SRVManager;
class Core;

/// <summary>
/// テクスチャを管理する
/// </summary>
class TextureManager {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
private://構造体
	//テクスチャデータ
	struct TextureData {
		DirectX::TexMetadata metadata = {};//画像の幅や高さなどの情報
		ComPtr<ID3D12Resource>resource = {};//テクスチャリソース
		uint32_t srvIndex;//SRVインデックス
		ComPtr<ID3D12Resource>intermediateResource = nullptr;//アップロードするリソース
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = {};//SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = {};//描画コマンドに必要なGPUハンドル

		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COPY_DEST; // ←生成直後はこれ
	};
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextureManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">SRVマネージャー</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャのファイルパス</param>
	void LoadTexture(std::string& filePath);

	/// <summary>
	/// テクスチャファイルのアンロード
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void UnloadTexture(const std::string& filePath);

	/// <summary>
	/// 文字テクスチャなどをCPUメモリから作成
	/// </summary>
	/// <param name="key">検索キー</param>
	/// <param name="pixelsBGRA">CPUメモリ上にあるピクセル配列の先頭アドレス</param>
	/// <param name="width">テクスチャの横幅(ピクセル単位)</param>
	/// <param name="height">テクスチャの縦幅(ピクセル単位)</param>
	/// <param name="strideBytes">1行当たりのバイト数</param>
	void CreateTextureFromMemoryBGRA(const std::string& key, const void* pixelsBGRA, uint32_t width, uint32_t height, uint32_t strideBytes);

	/// <summary>
	/// 文字テクスチャなどをCPUメモリからの更新
	/// </summary>
	/// <param name="key">検索キー</param>
	/// <param name="pixelsBGRA">CPUメモリ上にあるピクセル配列の先頭アドレス</param>
	/// <param name="width">テクスチャの横幅(ピクセル単位)</param>
	/// <param name="height">テクスチャの縦幅(ピクセル単位)</param>
	/// <param name="strideBytes">1行当たりのバイト数</param>
	void UpdateTextureFromMemoryBGRA(const std::string& key, const void* pixelsBGRA, uint32_t width, uint32_t height, uint32_t strideBytes);

	/// <summary>
	/// メタデータの取得
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>メタデータ</returns>
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

	/// <summary>
	/// SRVインデックスの取得
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>SRVインデックス</returns>
	uint32_t GetSRVIndex(const std::string& filePath);

	/// <summary>
	/// GPUハンドルの取得
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>GPUハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(const std::string& filePath);
public://PassKey
	class ConstructorKey {
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit TextureManager(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	TextureManager(TextureManager&) = delete;
	//代入演算子の禁止
	TextureManager& operator=(TextureManager&) = delete;
public://静的メンバ変数
	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;
private://メンバ変数
	//テクスチャデータ
	std::unordered_map<std::string, TextureData>textureDatas_;
	//DirectX基盤
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
};