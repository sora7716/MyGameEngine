#pragma once
#include "BaseDefenseObject.h"

/// <summary>
/// 壁
/// </summary>
class Wall :public BaseDefenseObject {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Wall(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Wall()override;

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
	/// 回復量を設定
	/// </summary>
	/// <param name="healAmount">回復量</param>
	void SetHealAmount(float healAmount);
private://メンバ関数
	/// <summary>
	/// 死亡したか確認
	/// </summary>
	void CheckDeath();

	/// <summary>
	/// 自己回復
	/// </summary>
	void SelfHeal();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//回復量
	float healAmount_ = 0.0f;
	//回復間隔(秒)
	float healInterval_ = 2.0f;
	//回復までの時間(秒)
	float healTimer_ = 0.0f;
};