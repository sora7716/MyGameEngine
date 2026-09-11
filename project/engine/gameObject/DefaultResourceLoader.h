#pragma once
#include <memory>

//前方宣言
class ModelManager;
class TextureManager;
class AudioManager;

/// <summary>
/// ゲームで使うリソースを管理
/// </summary>
class DefaultResourceLoader{
public://PassKey
	class ConstructorKey{
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="modelManager">モデルの管理</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="audioManager">オーディオの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<DefaultResourceLoader>Create(ConstructorKey key, ModelManager* modelManager, TextureManager* textureManager, AudioManager* audioManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit DefaultResourceLoader(ConstructorKey);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DefaultResourceLoader();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelManager">モデルの管理</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="audioManager">オーディオの管理</param>
	void Initialize(ModelManager* modelManager, TextureManager* textureManager, AudioManager* audioManager);
private://メンバ関数
	//コピーコンストラクタ禁止
	DefaultResourceLoader(const DefaultResourceLoader&) = delete;
	//代入演算子の禁止
	DefaultResourceLoader& operator=(const DefaultResourceLoader&) = delete;

	/// <summary>
	/// オーディオの読み込み
	/// </summary>
	void LoadAudio();

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	void LoadTexture();

	/// <summary>
	/// OBJファイルの読み込み
	/// </summary>
	void LoadModel();
private://メンバ変数
	//モデルの管理
	ModelManager* modelManager_ = nullptr;
	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;
	//オーディオの管理
	AudioManager* audioManager_ = nullptr;
};

