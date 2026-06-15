#pragma once
#include "ResourceData.h"
#include "algorithms/Rendering.h"
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <memory>

//前方宣言
class ModelCommon;
class DirectXBase;
class Mesh;

/// <summary>
/// モデル
/// </summary>
class Model {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
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
	/// モデルの生成(ファイルを読み込み)
	/// </summary>
	/// <param name="modelCommon">モデルの共通部分</param>
	/// <param name="storedFilePath">モデルを保管しているファイル名(最初と最後に"/"入らない)</param>
	/// <param name="filename">ファイル名(最初に"/"入らない</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateFromModel(ModelCommon* modelCommon, const std::string& storedFilePath, const std::string& filename);

	/// <summary>
	/// モデルの生成(キューブ)
	/// </summary>
	/// <param name="modelCommon">モデルの共通部分</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateCube(ModelCommon* modelCommon);

	/// <summary>
	/// モデルの生成(モデルデータ)
	/// </summary>
	/// <param name="modelCommon">モデルの共通部分</param>
	/// <param name="modelData">モデルデータ</param>
	/// <returns>モデル</returns>
	static std::unique_ptr<Model> CreateModelFromModelData(ModelCommon* modelCommon, const ModelData& modelData);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelCommon">モデルの共通部分</param>
	void Initialize(ModelCommon* modelCommon);

	/// <summary>
	/// メッシュの再構成
	/// </summary>
	/// <param name="meshes">メッシュ</param>
	void RebuildMeshes(const std::vector<MeshData>& meshes);

	/// <summary>
	/// 頂点を合成する
	/// </summary>
	/// <param name="rate">どれくらい合成するかの割合</param>
	/// <param name="minGridSize">グリッドサイズの最小値</param>
	/// <param name="maxGridSize">グリッドサイズの最大値</param>
	/// <returns>合成後のメッシュデータ</returns>
	std::vector<MeshData> VertexClustering(float rate, float min = 0.001f, float max = 1.0f);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="objectCount">表示したいオブジェクト数</param>
	void Draw(uint32_t objectCount = 1);

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
	/// <param name="imageName">画像の名前</param>
	void SetTexture(uint32_t materialIndex, const std::string& imageName);

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
	/// .mtlファイルの読み取り	
	/// </summary>
	/// <param name="directoryPath">ディレクトリファイルパス</param>
	/// <param name="filename">ファイル名</param>
	/// <returns>マテリアルデータ</returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ディレクトリファイルパス(最後に"/"はいらない)</param>
	/// <param name="storedFilePath">モデルを保管しているファイル名(最初と最後に"/"入らない)</param>
	/// <param name="filename">ファイル名(最初に"/"入らない</param>
	/// <returns>モデルデータ</returns>
	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& storedFilePath, const std::string& filename);

	/// <summary>
	/// マテリアルのセッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <param name="materialData">マテリアルデータ</param>
	void SetMaterial(uint32_t index, const Material& materialData);

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
	/// モデルの共通部分の取得
	/// </summary>
	/// <returns>モデルの共通部分</returns>
	ModelCommon* GetModelCommon();
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
	/// キューブの作成
	/// </summary>
	MeshData MakeCubeData();

	/// <summary>
	/// キューブの生成
	/// </summary>
	void CreateCube();

	/// <summary>
	/// モデルの生成
	/// </summary>
	/// <param name="storedFilePath">モデルを保管しているファイル名(最初と最後に"/"入らない)</param>
	/// <param name="filename">ファイル名(最初に"/"入らない</param>
	void CreateFromModel(const std::string& storedFilePath, const std::string& filename);

	/// <summary>
	/// モデルの生成(モデルデータ)
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	void CreateModelFromModelData(const ModelData& modelData);

	/// <summary>
	/// 各種リソースの生成
	/// </summary>
	void CreateResources();

	/// <summary>
	/// 近くにある頂点をまとめる
	/// </summary>
	/// <param name="size">グリッドサイズ</param>
	/// <returns>メッシュデータ</returns>
	MeshData VertexClusteringByGridSize(const MeshData& meshData, float size);
private://メンバ変数
	//ModelCommonのポインタ
	ModelCommon* modelCommon_ = nullptr;
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
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
};

