#pragma once
#include "Component.h"

//前方宣言
class Health;

/// <summary>
/// HPバー
/// </summary>
class HealthBar :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit HealthBar(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~HealthBar()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="health">HPの管理</param>
	void Setup(Health* health);

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

	/// <summary>
	/// 満タン時のスケールを取得
	/// </summary>
	/// <returns>満タン時のスケール</returns>
	float GetFullBarScale();
private://定数
	//メッシュの幅
	static inline const float kBarMeshWidth = 0.5f;
private://メンバ変数
	//Health
	Health* health_ = nullptr;
	//満タン時のスケール
	float fullBarScale = 1.0f;
};

