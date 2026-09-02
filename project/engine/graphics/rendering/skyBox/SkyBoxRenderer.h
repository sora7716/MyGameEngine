#pragma once
#include "SkyBoxRenderData.h"
#include "Vector4.h"
#include "Vector3.h"
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <memory>

//前方宣言
class DirectXBase;
class TextureManager;
class Camera;

//頂点情報
struct SkyBoxVertexData{
	Vector4 vertex;
	Vector3 texcoord;
};

/// <summary>
/// スカイボックスのレンダラー
/// </summary>
class SkyBoxRenderer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">Textureの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<SkyBoxRenderer>Create(DirectXBase* directXBase, TextureManager* textureManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SkyBoxRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SkyBoxRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャの管理</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager);

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データの追加</param>
	void AddRenderData(const SkyBoxRenderData& renderData);

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	void Draw(uint32_t instanceIndex);

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode(uint32_t instanceIndex);

	/// <summary>
	/// 描画データのサイズの取得
	/// </summary>
	/// <returns>描画データのサイズ</returns>
	uint32_t GetRenderDataSize();
private://メンバ関数
	/// <summary>
	/// 頂点データの初期化
	/// </summary>
	void InitializeVertexData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスデータの初期化
	/// </summary>
	void InitializeIndexData();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// ワールド行列リソースの生成
	/// </summary>
	void CreateWorldMatrixResource();
private://定数
	//頂点数
	static inline const uint32_t kVertexCount = 24;
	//インデックス数
	static inline const uint32_t kIndexCount = 36;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;

	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;

	//頂点情報
	std::vector<SkyBoxVertexData>vertices_;
	//頂点バッファ
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;
	//VBV
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};

	//インデックス情報
	std::vector<uint32_t>indices_;
	//インデックスバッファ
	ComPtr<ID3D12Resource>indexResource_ = nullptr;
	//IBV
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	//マテリアル情報
	Vector4* materialData_ = nullptr;
	//マテリアルのCBuffer
	ComPtr<ID3D12Resource>materialResource_ = nullptr;

	//ワールド行列の情報
	Matrix4x4* worldMatrix_ = nullptr;
	//ワールド行列のCBuffer
	ComPtr<ID3D12Resource>worldMatrixResource_ = nullptr;

	//スカイボックスの描画データ
	std::vector<SkyBoxRenderData> renderDatas_ = {};
};

