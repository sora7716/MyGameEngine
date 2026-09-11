#pragma once
#include "TextureLoader.h"
#include "RenderData.h"
#include <unordered_map>
#include <memory>

//前方宣言
class DirectXBase;
class SRVManager;
class Core;

/// <summary>
/// テクスチャを管理する
/// </summary>
class TextureManager{
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
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">SRVマネージャー</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<TextureManager>Create(ConstructorKey key, DirectXBase* directXBase, SRVManager* srvManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit TextureManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextureManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="srvManager">SRVマネージャー</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager);

	/// <summary>
	/// テクスチャの追加
	/// </summary>
	/// <param name="filePath">テクスチャのファイルパス</param>
	void AddTexture(const std::string& filePath);

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
	std::unordered_map<std::string, TextureData> textureDatas_;
	//DirectX基盤
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
};