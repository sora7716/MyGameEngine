#pragma once
#include "Component.h"

/// <summary>
/// 敵
/// </summary>
class Enemy :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Enemy(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy()override;

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
	/// 接触した瞬間
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollisionEnter(const CollisionInfo& info);

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
	//敵の攻撃力
	float attackPower_ = 20.0f;
};

