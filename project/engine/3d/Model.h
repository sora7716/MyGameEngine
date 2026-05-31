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
	/// 初期化
	/// </summary>
	/// <param name="modelCommon">モデルの共通部分</param>
	/// <param name="directoryPath">ディレクトリファイルパス(最後に"/"はいらない)</param>
	/// <param name="storedFilePath">モデルを保管しているファイル名(最初と最後に"/"入らない)</param>
	/// <param name="filename">ファイル名(最初に"/"入らない</param>
	void Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& storedFilePath, const std::string& filename);


	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="objectCount">表示したいオブジェクト数</param>
	void Draw(uint32_t objectCount = 1);

	/// <summary>
	/// uv変換
	/// </summary>
	/// <param name="uvTransform">uv座標</param>
	void UVTransform(Transform2dData uvTransform);

	/// <summary>
	/// 色を変更
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);

	/// <summary>
	/// テクスチャの変更
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void SetTexture(const std::string& filePath);

	/// <summary>
	/// 色を取得
	/// </summary>
	/// <returns>色</returns>
	const Vector4& GetColor()const;

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
	/// <param name="materialData">マテリアルデータ</param>
	void SetMaterial(const Material& materialData);

	/// <summary>
	/// リムライトのセッター
	/// </summary>
	/// <param name="rimLight">リムライト</param>
	void SetRimLight(const RimLight& rimLight);
private://メンバ関数
	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// リムライトのリソースを生成
	/// </summary>
	void CreateRimLightResource();
private://メンバ変数
	//ModelCommonのポインタ
	ModelCommon* modelCommon_ = nullptr;
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//Objファイルデータ
	ModelData modelData_ = {};
	//マテリアルリソース
	ComPtr<ID3D12Resource>materialResource_ = nullptr;
	//マテリアルリソースにデータを書き込むためのポインタ
	Material* materialPtr_ = nullptr;
	//リムライト
	RimLight* rimLightPtr_ = nullptr;
	//リムライトリソース
	ComPtr<ID3D12Resource>rimLightResource_ = nullptr;
	//メッシュ
	std::unique_ptr<Mesh>mesh_ = nullptr;
};

