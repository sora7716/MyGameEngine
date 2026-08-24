#pragma once
#include "SpriteRenderData.h"
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <memory>

//前方宣言
class DirectXBase;
class TextureManager;

/// <summary>
/// スプライトのレンダラー
/// </summary>
class SpriteRenderer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">Textureの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<SpriteRenderer>Create(DirectXBase* directXBase, TextureManager* textureManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SpriteRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpriteRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">Textureの管理</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager);


	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const SpriteRenderData& renderData);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	void Draw(uint32_t instanceIndex);
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
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// トランスフォーメーション行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();
private://定数
	//頂点数
	static inline const uint32_t kVertexCount = 4;
	//インデックス数
	static inline const uint32_t kIndexCount = 6;
private://メンバ変数
	//DirectXの基盤
	DirectXBase* directXBase_ = nullptr;

	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;

	//頂点情報
	std::vector<VertexData>vertices_;
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
	MaterialForSprite* materialData_ = nullptr;
	//マテリアルのCBuffer
	ComPtr<ID3D12Resource>materialResource_ = nullptr;

	//トランスフォーメーション行列の情報
	TransformationMatrixForSprite* transformationMatrix_ = nullptr;
	//トランスフォーメーション行列のCBuffer
	ComPtr<ID3D12Resource>transformationMatrixResource_ = nullptr;

	//スプライトの描画データ
	std::vector<SpriteRenderData> renderDatas_ = {};
};