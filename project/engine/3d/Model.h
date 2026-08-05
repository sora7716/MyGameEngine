#pragma once
#include "RenderData.h"
#include "MatrixUtility.h"
#include "Object3dRendererData.h"
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <memory>

//前方宣言
class DirectXBase;
class TextureManager;
class Mesh;

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
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="modelFileName">モデルのファイル名</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModel(DirectXBase* directXBase, TextureManager* textureManager, const std::string& modelFileName);

	/// <summary>
	/// モデルの生成(メッシュデータ)
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="meshDatas">メッシュデータ</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModel(DirectXBase* directXBase, TextureManager* textureManager, const std::vector<MeshData>& meshDatas);

	/// <summary>
	/// モデルの生成(モデルデータ)
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="modelData">モデルデータ</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModel(DirectXBase* directXBase, TextureManager* textureManager, const ModelData& modelData);
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
	/// <param name="textureManager">テクスチャの管理</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager);

	/// <summary>
	/// メッシュの再構成
	/// </summary>
	/// <param name="meshes">メッシュ</param>
	void RebuildMeshes(const std::vector<MeshData>& meshes);

	/// <summary>
	/// 描画に必要なデータのセットアップ
	/// </summary>
	void SetupRenderData();

	/// <summary>
	/// UV変換
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="uvTransform">uv座標</param>
	void UVTransform(uint32_t index, Transform2d uvTransform);

	/// <summary>
	/// 色を変更
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t index, const Vector4& color);

	/// <summary>
	/// テクスチャの変更
	/// </summary>
	/// <param name="materialIndex">マテリアルインデックス</param>
	/// <param name="imageFileName">画像のファイル名</param>
	void SetTexture(uint32_t materialIndex, const std::string& imageFileName);

	/// <summary>
	/// 環境マップの変更
	/// </summary>
	/// <param name="materialIndex">マテリアルインデックス</param>
	/// <param name="environmentMapFileName">環境マップのファイル名</param>
	void SetEnvironmentMap(uint32_t materialIndex, const std::string& environmentMapFileName);

	/// <summary>
	/// 色を取得
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>色</returns>
	const Vector4& GetColor(uint32_t index)const;

	/// <summary>
	/// モデルデータのゲッター
	/// </summary>
	/// <returns>モデルデータ</returns>
	const ModelData& GetModelData()const;

	/// <summary>
	/// ライティングの設定
	/// </summary>
	/// <param name="index">マテリアルの検索キー</param>
	/// <param name="isLighting">ライティングフラグ</param>
	void SetIsLighting(uint32_t materialIndex, bool isLighting);

	/// <summary>
	/// 輝度の設定
	/// </summary>
	/// <param name="index">マテリアルの検索キー</param>
	/// <param name="shininess">輝度</param>
	void SetShininess(uint32_t materialIndex, float shininess);

	/// <summary>
	/// 環境マップの映り込み度を調整
	/// </summary>
	/// <param name="materialIndex">マテリアルの検索キー</param>
	/// <param name="environmentCoefficient">環境マップの映り込み度</param>
	void SetEnvironmentCoefficient(uint32_t materialIndex, float environmentCoefficient);

	/// <summary>
	/// リムライトのセッター
	/// </summary>
	/// <param name="rimLight">リムライト</param>
	void SetRimLight(const RimLight& rimLight);

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
private://メンバ関数
	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// リムライトのリソースを生成
	/// </summary>
	void CreateRimLightResource();

	/// <summary>
	/// メッシュの構築
	/// </summary>
	void BuildMesh();

	/// <summary>
	/// モデルの作成(メッシュデータから1)
	/// </summary>
	/// <param name="meshDatas">メッシュデータ</param>
	void CreateModel(const std::vector<MeshData>& meshDatas, const std::string& nodeName = "primitive");

	/// <summary>
	/// モデルの生成(モデルのファイルから)
	/// </summary>
	/// <param name="objectFileName">オブジェクトのファイル名</param>
	void CreateModel(const std::string& objectFileName);

	/// <summary>
	/// モデルの生成(モデルデータ)
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	void CreateModel(const ModelData& modelData);

	/// <summary>
	/// 各種リソースの生成
	/// </summary>
	void CreateResources();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;
	//メッシュ
	std::vector<std::unique_ptr<Mesh>>meshes_;
	//Objファイルデータ
	ModelData modelData_ = {};
	//マテリアルリソース
	std::vector<ComPtr<ID3D12Resource>>materialResources_;
	//マテリアルリソースにデータを書き込むためのポインタ
	std::vector<Material*> materialPtrs_;
	//リムライト
	RimLight* rimLightPtr_ = nullptr;
	//リムライトリソース
	ComPtr<ID3D12Resource>rimLightResource_ = nullptr;
	//描画に必要なデータ
	ModelRenderData modelRenderData_ = {};
};

