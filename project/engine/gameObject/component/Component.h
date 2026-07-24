#pragma once
#include <memory>
//前方宣言
class GameObject;

/// <summary>
/// コンポーネント
/// </summary>
class Component{
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual~Component();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	virtual std::unique_ptr<Component>& Clone(GameObject* gameObject)const = 0;

	/// <summary>
	/// 取り付け先を取得
	/// </summary>
	/// <returns>取り付け先</returns>
	GameObject* GetOwner()const;

	/// <summary>
	/// 有効状態の設定
	/// </summary>
	/// <param name="isEnabled">有効状態</param>
	void SetEnabled(bool isEnabled);

	/// <summary>
	/// 有効状態の取得
	/// </summary>
	/// <returns>有効状態</returns>
	bool IsEnabled()const;
protected://メンバ 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="owner">ゲームオブジェクト</param>
	explicit Component(GameObject* owner);
private://メンバ変数
	//Componentが取り付けられているゲームオブジェクト
	GameObject* owner_ = nullptr;

	//Component単体の有効状態
	bool isEnabled_ = true;
};

