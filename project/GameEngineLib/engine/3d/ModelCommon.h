//前方宣言
class DirectXBase;
class TextureManager;

//3dモデルの共通部分
class ModelCommon{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ModelCommon() = default;
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ModelCommon() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャマネージャー</param>
	void Initialize(DirectXBase* directXBase,TextureManager*textureManager);

	/// <summary>
	/// DirectXの基盤部分のゲッター
	/// </summary>
	/// <returns>DirectXの基盤部分</returns>
	DirectXBase* GetDirectXBase()const;

	/// <summary>
	/// テクスチャマネージャーの記録
	/// </summary>
	/// <returns>テクスチャマネージャー</returns>
	TextureManager* GetTextureManager();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;
};