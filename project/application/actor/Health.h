#pragma once
#include "Component.h"

/// <summary>
/// HP管理
/// </summary>
class Health :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Health(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Health()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// HPからダメージ分減らす処理
	/// </summary>
	/// <param name="damage">ダメージ</param>
	void ApplyDamage(float damage);

	/// <summary>
	/// HPを回復させる
	/// </summary>
	/// <param name="heal">回復する値</param>
	void Heal(float heal);

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry);

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
	/// HPの最大値を設定
	/// </summary>
	/// <param name="maxHp">最大値</param>
	void SetMaxHp(float maxHp);

	/// <summary>
	/// 死亡フラグの取得
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool IsDead()const;

	/// <summary>
	/// 現在のHPを取得
	/// </summary>
	/// <returns>現在のHP</returns>
	float GetCurrentHp()const;

	/// <summary>
	/// 最大のHPを取得
	/// </summary>
	/// <returns>最大のHP</returns>
	float GetMaxHp()const;
private://メンバ変数
	//最大HP
	float maxHp_ = 0.0f;
	//現在のHP
	float currentHp_ = 0.0f;
};

