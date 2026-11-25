#pragma once
#include "Component.h"
#include "Vector3.h"

//前方宣言
class Object3d;
class MaterialInstance;

/// <summary>
///	落雷
/// </summary>
class LightningStrike :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit LightningStrike(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LightningStrike()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry)override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
	//生存時間
	float lifeTimer_ = 0.0f;
	//最大で生存できる時間
	float maxLifetime_ = 0.1f;
	//フラッシュのマテリアル
	std::shared_ptr<MaterialInstance>lightningStrikeMat_ = nullptr;
	//最初のカラー
	Color beginColor_ = {};
	//最終的なカラー
	Color targetColor_ = {};
};

