#pragma once
#include "RenderData.h"
#include "MatrixUtility.h"
#include "Object3dRenderData.h"
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <memory>

//前方宣言
class DirectXBase;
class Mesh;
class MaterialInstance;
class LODBuilder;
class ShaderManager;

/// <summary>
/// モデル
/// </summary>
class Model{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://静的メンバ関数
	/// <summary>
	/// モデルの生成(ファイルを読み込み)
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="modelFileName">モデルのファイル名</param>
	/// <param name="shaderManager">シェーダの管理</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModel(DirectXBase* directXBase, const std::string& modelFileName, ShaderManager* shaderManager);

	/// <summary>
	/// モデルの生成(メッシュデータ)
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="meshDatas">メッシュデータ</param>
	/// <param name="shaderManager">シェーダの管理</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModel(DirectXBase* directXBase, const std::vector<MeshData>& meshDatas, ShaderManager* shaderManager);

	/// <summary>
	/// モデルの生成(モデルデータ)
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="modelData">モデルデータ</param>
	/// <param name="shaderManager">シェーダの管理</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModel(DirectXBase* directXBase, const ModelData& modelData, ShaderManager* shaderManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Model();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="shaderManager">シェーダの管理</param>
	void Initialize(DirectXBase* directXBase,ShaderManager*shaderManager);

	/// <summary>
	/// メッシュの再構成
	/// </summary>
	/// <param name="meshes">メッシュ</param>
	void RebuildMeshes(const std::vector<MeshData>& meshes);

	/// <summary>
	/// LODモデルの生成
	/// </summary>
	/// <param name="keepRates">ポリゴン数の割合をまとめたもの</param>
	void CreateLODModels(const std::vector<float>& keepRates);

	/// <summary>
	/// LODモデルの取得
	/// </summary>
	/// <param name="lodIndex">LODモデルの検索キー</param>
	/// <returns>LODモデル</returns>
	Model* GetLODModel(uint32_t lodIndex);

	/// <summary>
	/// LODの数の取得
	/// </summary>
	/// <returns>LODの数</returns>
	uint32_t GetLODCount()const;

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();

	/// <summary>
	/// モデルデータのゲッター
	/// </summary>
	/// <returns>モデルデータ</returns>
	const ModelData& GetModelData()const;

	/// <summary>
	/// メッシュ達の取得
	/// </summary>
	/// <returns>メッシュ達</returns>
	const std::vector<std::unique_ptr<Mesh>>& GetMeshes()const;

	/// <summary>
	/// 描画に必要なデータの取得
	/// </summary>
	/// <returns>描画に必要なデータ</returns>
	const ModelRenderData& GetModelRenderData();

	/// <summary>
	/// デフォルトのマテリアルインスタンスの取得
	/// </summary>
	/// <returns>デフォルトのマテリアルインスタンス</returns>
	std::shared_ptr<MaterialInstance>GetDefaultMaterialInstance()const;
private://メンバ関数
	/// <summary>
	/// メッシュの構築
	/// </summary>
	void BuildMesh();

	/// <summary>
	/// モデルの作成(メッシュデータから)
	/// </summary>
	/// <param name="meshDatas">メッシュデータ</param>
	/// <param name="shaderManager">シェーダの管理</param>
	/// <param name="nodeName">ノード名</param>
	void CreateModel(const std::vector<MeshData>& meshDatas, ShaderManager* shaderManager, const std::string& nodeName = "primitive");

	/// <summary>
	/// モデルの生成(モデルのファイルから)
	/// </summary>
	/// <param name="objectFileName">オブジェクトのファイル名</param>
	/// <param name="shaderManager">シェーダの管理</param>
	void CreateModel(const std::string& objectFileName, ShaderManager* shaderManager);

	/// <summary>
	/// モデルの生成(モデルデータ)
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	/// <param name="shaderManager">シェーダの管理</param>
	void CreateModel(const ModelData& modelData, ShaderManager* shaderManager);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;

	//シェーダの管理
	ShaderManager* shaderManager_ = nullptr;

	//メッシュ
	std::vector<std::unique_ptr<Mesh>>meshes_;

	//Objファイルデータ
	ModelData modelData_ = {};

	//マテリアルインスタンス
	std::shared_ptr<MaterialInstance>defaultMaterialInstance_ = nullptr;

	//描画に必要なデータ
	ModelRenderData modelRenderData_ = {};

	//LODのビルダー
	std::unique_ptr<LODBuilder>lodBuilder_ = nullptr;

	//LODを作成したかのフラグ
	bool isLODGenerated_ = false;

	//LOD倍率を保存
	std::vector<float>generatedLODRates_;
};

