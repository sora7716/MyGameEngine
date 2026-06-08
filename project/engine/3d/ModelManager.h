#pragma once
#include <map>
#include <string>
#include <memory>

//前方宣言
class DirectXBase;
class ModelCommon;
class Model;
class TextureManager;

/// <summary>
/// モデルの管理
/// </summary>
class ModelManager {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ModelManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelCommon">モデルの共通部分</param>
	void Initialize(ModelCommon* modelCommon);

	/// <summary>
	/// objモデルの読み込み
	/// </summary>
	/// <param name="name">名前</param>
    /// <param name="storedFileName">保管しているファイル名("/"は書かなくていい)</param>
	/// <param name="filePath">ファイル名("/"と".obj"は書かなくていい)</param>
	void LoadModel(const std::string& name, const std::string& storedFileName, const std::string& filePath);

	/// <summary>
	/// モデルの検索(.objはいらない)
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>モデル</returns>
	Model* FindModel(const std::string& name);

	/// <summary>
	/// モデルの共通部分のゲッター
	/// </summary>
	/// <returns>モデルの共通部分</returns>
	ModelCommon* GetModelCommon();
public://PassKey
	class ConstructorKey {
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit ModelManager(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	ModelManager(const ModelManager&) = delete;
	//代入演算子禁止
	ModelManager operator=(const ModelManager&) = delete;
private://メンバ変数
	//モデルデータコンテナ
	std::map<std::string, std::unique_ptr<Model>>models_;
	//モデルの共通部分
	ModelCommon* modelCommon_ = nullptr;
};

