#pragma once
#include "ResourceData.h"
#include <wrl.h>
#include <d3d12.h>

//前方宣言
class DirectXBase;

/// <summary>
/// メッシュ
/// </summary>
class Mesh{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Mesh();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Mesh();

	/// <summary>
    /// 初期化
    /// </summary>
    /// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="meshData">メッシュデータ</param>
	void Initialize(DirectXBase*directXBase,const MeshData& meshData);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="objectCount">オブジェクト数</param>
	void Draw(uint32_t objectCount);

	/// <summary>
	/// マテリアルインデックスの取得
	/// </summary>
	/// <returns>マテリアルインデックス</returns>
	uint32_t GetMaterialIndex();
private://メンバ関数
	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//VertexResource
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;
	//VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	//IndexResource
	ComPtr<ID3D12Resource>indexResource_ = nullptr;
	//IndexBufferView
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
	//メッシュデータ
	MeshData meshData_;
};

