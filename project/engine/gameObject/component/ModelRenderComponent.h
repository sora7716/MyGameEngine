//#pragma once
//#include <string>
//#include <memory>
//#include "RenderData.h"
//#include "Component.h"
//
////前方宣言
//class Object3d;
//
///// <summary>
///// モデル描画用のコンポーネント
///// </summary>
//class ModelRenderComponent :Component{
//public://メンバ関数
//	/// <summary>
//	/// コンストラクタ
//	/// </summary>
//	ModelRenderComponent();
//
//	/// <summary>
//	/// デストラクタ
//	/// </summary>
//	~ModelRenderComponent()override;
//
//	/// <summary>
//	/// 初期化
//	/// </summary>
//	void Initialize()override;
//
//	/// <summary>
//	/// 更新
//	/// </summary>
//	void Update()override;
//
//	/// <summary>
//	/// コピー
//	/// </summary>
//	/// <param name="gameObject"></param>
//	/// <returns></returns>
//	std::unique_ptr<Component>& Clone(GameObject* gameObject)const override;
//
//	/// <summary>
//	/// マテリアルの設定
//	/// </summary>
//	/// <param name="material">マテリアル</param>
//	void SetMaterial(const Material& material);
//
//	/// <summary>
//	/// モデルの設定
//	/// </summary>
//	/// <param name="modelName">モデル名</param>
//	void SetModel(const std::string& modelName);
//
//	/// <summary>
//	/// テクスチャの設定
//	/// </summary>
//	/// <param name="textureName">テクスチャ名</param>
//	void SetTexture(const std::string& textureName);
//
//	/// <summary>
//	/// 環境マップの設定
//	/// </summary>
//	/// <param name="environmentMapName">環境マップ名</param>
//	void SetEnvironmentMap(const std::string& environmentMapName);
//private://メンバ変数
//	//モデル名
//	std::string modelName_ = "";
//	//オブジェクト3d
//	std::unique_ptr<Object3d>object3d_ = nullptr;
//	//マテリアル
//	Material material_ = {};
//	//テクスチャパス
//	MaterialTexturePaths materialTexturePath_ = {};
//};
//
