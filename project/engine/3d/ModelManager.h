#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>

//前方宣言
class DirectXBase;
class TextureManager;
class Model;

/// <summary>
/// モデルの管理
/// </summary>
class ModelManager{
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ModelManager>Create(ConstructorKey key, DirectXBase* directXBase, TextureManager* textureManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit ModelManager(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ModelManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャの管理</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager);

	/// <summary>
	/// プリミティブなモデルの生成
	/// </summary>
	void CreatePrimitiveModel();

	/// <summary>
	/// モデルの追加
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="modelFileName">モデルのファイル名</param>
	void AddModel(const std::string& name, const std::string& modelFileName);

	/// <summary>
	/// モデルの検索(.objはいらない)
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>モデル</returns>
	Model* FindModel(const std::string& name);
private://メンバ関数
	//コピーコンストラクタ禁止
	ModelManager(const ModelManager&) = delete;
	//代入演算子禁止
	ModelManager operator=(const ModelManager&) = delete;
private://メンバ変数
	//モデルのコンテナ
	std::map<std::string, std::unique_ptr<Model>>models_;
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//Textureの管理
	TextureManager* textureManager_ = nullptr;
};

