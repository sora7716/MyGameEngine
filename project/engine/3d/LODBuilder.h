#pragma once
#include "RenderData.h"
#include "Vector4.h"
#include <memory>
#include <vector>
#include <cstdint>
#include <string>

//前方宣言
class DirectXBase;
class TextureManager;
class Model;

/// <summary>
/// LODモデルの生成
/// </summary>
class LODBuilder{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LODBuilder();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LODBuilder();

	/// <summary>
	/// LODモデルの生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="model">モデル</param>
	/// <param name="keepRates">頂点合成する割合</param>
	/// <returns>LODモデル</returns>
	void CreateLODModel(DirectXBase* directXBase, Model* model, const std::vector<float>& keepRates);

	/// <summary>
	/// LODモデルの取得
	/// </summary>
	/// <param name="lodIndex">LODモデルの検索キー</param>
	/// <returns>LODモデル</returns>
	Model* GetLODModel(uint32_t lodIndex);

	/// <summary>
	/// LODモデルの配列を取得
	/// </summary>
	/// <returns>LODモデルの配列</returns>
	const std::vector<std::unique_ptr<Model>>& GetLODModels()const;

	/// <summary>
	/// モデルのサイズを取得
	/// </summary>
	/// <returns>モデルのサイズ</returns>
	uint32_t LODModelSize()const;

	/// <summary>
	/// グリッドサイズの最小値
	/// </summary>
	/// <param name="minGridSize">グリッドサイズの最小値</param>
	void SetMinGridSize(float minGridSize);

	/// <summary>
	/// グリッドサイズの最大値の設定
	/// </summary>
	/// <param name="maxGridSize"></param>
	void SetMaxGridSize(float maxGridSize);
private://メンバ関数
	/// <summary>
	/// 辺縮約
	/// </summary>
	/// <param name="meshData">メッシュデータ</param>
	/// <param name="rate">倍率</param>
	/// <returns>辺縮約したメッシュデータ</returns>
	std::vector<MeshData> EdgeCollapse(const std::vector<MeshData>& meshData, float rate);

	/// <summary>
	/// 近くにある頂点をまとめる
	/// </summary>
	/// <param name="size">グリッドサイズ</param>
	/// <returns>メッシュデータ</returns>
	MeshData VertexClusteringByGridSize(const MeshData& meshData, float size);

	/// <summary>
	/// 頂点合成
	/// </summary>
	/// <param name="meshData">メッシュデータ</param>
	/// <param name="rate"></param>
	/// <returns></returns>
	std::vector<MeshData> VertexClustering(const std::vector<MeshData>& meshData, float rate);
private://メンバ変数
	//LOD用のモデル
	std::vector<std::unique_ptr<Model>>lodModels_;
	//頂点合成をする際のグリッドサイズの最大最小値
	float minGridSize_ = 0.001f;
	float maxGridSize_ = 1.0f;
};

