#pragma once
//前方宣言
class Model;
#include <memory>
#include <vector>
#include <cstdint>
#include <string>
#include <Vector4.h>

/// <summary>
/// LODモデルの生成
/// </summary>
class LODBuilder {
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
	/// <param name="model">モデル</param>
	/// <param name="keepRates">頂点合成する割合</param>
	/// <returns>LODモデル</returns>
	void CreateLODModel(Model* model, const std::vector<float>& keepRates);

	/// <summary>
	/// LODモデルの取得
	/// </summary>
	/// <param name="lodIndex">LODモデルの検索キー</param>
	/// <returns>LODモデル</returns>
	Model* GetLODModel(uint32_t lodIndex);

	/// <summary>
	/// カラーの設定
	/// </summary>
	/// <param name="materialIndex">マテリアルの検索キー</param>
	/// <param name="color">色</param>
	void SetColor(uint32_t materialIndex, const Vector4& color);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="materialIndex">マテリアルの検索キー</param>
	/// <param name="filePath">ファイルパス</param>
	void SetTexture(uint32_t materialIndex, const std::string& filePath);

	/// <summary>
	/// モデルのサイズを取得
	/// </summary>
	/// <returns>モデルのサイズ</returns>
	uint32_t LODModelSize()const;
private://メンバ変数
	//LOD用のモデル
	std::vector<std::unique_ptr<Model>>lodModels_;
};

