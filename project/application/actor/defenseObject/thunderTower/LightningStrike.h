#pragma once
#include "Component.h"
#include "EnemyData.h"

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
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other)override;

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

	/// <summary>
	/// ショックのパラメータを取得
	/// </summary>
	/// <returns>ショックのパラメータ</returns>
	const ShockParameter& GetShockParameter()const;

	/// <summary>
	/// ショックパラメータを設定
	/// </summary>
	/// <param name="parameter">ショックパラメータ</param>
	void SetShockParameter(const ShockParameter& parameter);
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
	//ショックステート
	ShockParameter parameter_ = {};
};

