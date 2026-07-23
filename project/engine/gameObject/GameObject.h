#pragma once
#include "RenderingData.h"
#include <vector>
#include <string>
#include <memory>

//前方宣言
class Component;

//ゲームオブジェクト
class GameObject{
public://静的メンバ関数
	/// <summary>
	/// ゲームオブジェクトの生成
	/// </summary>
	/// <param name="name">ゲームオブジェクトの名前</param>
	/// <returns></returns>
	static std::unique_ptr<GameObject>Create(const std::string& name);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameObject();

	/// <summary>
	/// コピーコンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	GameObject(const GameObject& gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameObject();

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns>複製したゲームオブジェクト</returns>
	std::unique_ptr<GameObject> Clone()const;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="name">ゲームオブジェクトの名前</param>
	void Initialize(const std::string& name);

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	/// <returns>トランスフォーム</returns>
	Transform& GetTransform();

	/// <summary>
	/// トランスフォームの取得
	/// </summary>
	/// <returns>トランスフォーム</returns>
	const Transform& GetTransform()const;

	/// <summary>
	/// アクティブかどうかを設定
	/// </summary>
	/// <param name="isActive"></param>
	void SetIsActive(bool isActive);

	/// <summary>
	/// アクティブかどうかを取得
	/// </summary>
	/// <returns></returns>
	bool IsActive()const;

	/// <summary>
	/// 名前の設定
	/// </summary>
	/// <param name="name">名前</param>
	void SetName(const std::string& name);

	/// <summary>
	/// 名前を取得
	/// </summary>
	/// <returns></returns>
	const std::string& GetName()const;

	/// <summary>
	/// タグの設定
	/// </summary>
	/// <param name="tag">タグ</param>
	void SetTag(const std::string& tag);

	/// <summary>
	/// タグの取得
	/// </summary>
	/// <returns>タグ</returns>
	const std::string& GetTag()const;

	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <param name="component">コンポーネント</param>
	void AddComponent(std::unique_ptr<Component> component);

	/// <summary>
	/// すべてのコンポーネントの更新
	/// </summary>
	void UpdateComponents();

	/// <summary>
	/// コンポーネントをすべて取得
	/// </summary>
	/// <returns>すべてのコンポーネント</returns>
	const std::vector<std::unique_ptr<Component>>& GetComponents()const;
private://メンバ変数
	//名前
	std::string name_ = "\0";
	//SRT
	Transform transform_ = {};
	//有効か
	bool isActive_ = false;
	//タグ
	std::string tag_;
	//コンポーネント
	std::vector<std::unique_ptr<Component>>components_;
};