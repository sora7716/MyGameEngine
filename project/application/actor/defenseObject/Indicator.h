#pragma once
#include "Component.h"

/// <summary>
/// インディケータ
/// </summary>
class Indicator :public Component {
public://列挙型
	//振る舞い
	enum Behavior {
		kNone,
		kVisibility,//表示
		kHidden//非表示
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Indicator(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Indicator()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other);

	/// <summary>
	/// 離れた瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerExit(BaseCollider* other);

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
	//振る舞いの要求
	Behavior behaviorRequest_ = Behavior::kHidden;
	//振る舞い
	Behavior behavior_ = Behavior::kNone;
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
};

