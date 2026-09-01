#pragma once
#include "RenderingData.h"
#include "Component.h"
#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include <utility>

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
	/// <typeparam name="T">コンポーネントの型</typeparam>
	/// <returns>コンポーネントのポインタ</returns>
	template<class T, class... Args >
	T* AddComponent(Args&&... args){
		static_assert(
			std::is_base_of_v<Component, T>,
			"TはComponentを継承している必要があります"
		);

		//Componentを生成
		std::unique_ptr<T>component = std::make_unique<T>(this, std::forward<Args>(args)...);

		//返却値のポインタを保存する
		T* componentPtr = component.get();

		//GameObjectに所有させる
		components_.push_back(std::move(component));

		//初期化する
		componentPtr->Initialize();

		return componentPtr;
	}

	/// <summary>
	/// コンポーネントの取得
	/// </summary>
	/// <typeparam name="T">コンポーネントの型</typeparam>
	/// <returns>コンポーネントのポインタ</returns>
	template <class T>
	T* GetComponent(){
		static_assert(
			std::is_base_of_v<Component, T>,
			"TはComponentを継承している必要があります"
		);

		for (const std::unique_ptr<Component>& component : components_){
			T* target = dynamic_cast<T*>(component.get());

			if (target){
				return target;
			}
		}

		//指定されたComponentが無かった
		return nullptr;
	}

	/// <summary>
	/// コンポーネントの更新
	/// </summary>
	void UpdateComponents();
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