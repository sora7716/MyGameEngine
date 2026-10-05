#pragma once
#include "RenderingData.h"
#include "Component.h"
#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include <utility>

//前方宣言
class BaseScene;

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
	/// 現在の接続シーンの設定
	/// </summary>
	/// <param name="currentScene">現在の接続シーン</param>
	void SetCurrentScene(BaseScene* currentScene);

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
	/// 現在の接続シーンの取得
	/// </summary>
	/// <returns>現在の接続シーン</returns>
	BaseScene* GetCurrentScene();

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
	/// コンポーネントの取得
	/// </summary>
	/// <typeparam name="T">コンポーネントの型</typeparam>
	/// <returns>コンポーネントのポインタ</returns>
	template <class T>
	std::vector<T*> GetComponents(){
		static_assert(
			std::is_base_of_v<Component, T>,
			"TはComponentを継承している必要があります"
		);

		//対象の配列
		std::vector<T*>targets;

		for (const std::unique_ptr<Component>& component : components_){
			T* target = dynamic_cast<T*>(component.get());

			if (target){
				targets.push_back(target);
			}
		}

		//配列を返す
		return targets;
	}

	/// <summary>
	/// コンポーネントの更新
	/// </summary>
	/// <param name="phase">更新のフェーズ</param>
	void UpdateComponents(UpdatePhase phase);

	/// <summary>
	/// デバッグでImGuiを使用できるようにする
	/// </summary>
	void DebugImGui();

	/// <summary>
	/// 衝突判定のイベントを呼び出す
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="info">衝突判定の情報</param>
	void InvokeCollisionEvent(uint32_t index, const CollisionInfo& info);

	/// <summary>
	/// 衝突判定のイベントを呼び出す
	/// </summary>
	/// <param name="index">検索キー</param>
	/// <param name="other">衝突対象のコライダー</param>
	void InvokeTriggerEvent(uint32_t index, BaseCollider* other);

	/// <summary>
	/// ゲームオブジェクトから解除すると通知
	/// </summary>
	/// <param name="target">対象</param>
	void NotifyGameObjectRemoving(GameObject* target);
private://メンバ関数
	/// <summary>
	/// 接触した瞬間ということを各コンポーネントに通知する
	/// </summary>
	/// <param name="info">衝突したときの情報</param>
	void NotifyOnCollisionEnter(const CollisionInfo& info);

	/// <summary>
	/// 接触中ということを各コンポーネントに通知する
	/// </summary>
	/// <param name="info">衝突したときの情報</param>
	void NotifyOnCollisionStay(const CollisionInfo& info);

	/// <summary>
	/// 離れた瞬間ということを各コンポーネントに通知する
	/// </summary>
	/// <param name="info">衝突したときの情報</param>
	void NotifyOnCollisionExit(const CollisionInfo& info);

	/// <summary>
	/// 接触した瞬間ということを各コンポーネントに通知する
	/// </summary>
	/// <param name="other">衝突対象のコライダー</param>
	void NotifyOnTriggerEnter(BaseCollider* other);

	/// <summary>
	/// 接触中ということを各コンポーネントに通知する
	/// </summary>
	/// <param name="other">衝突対象のコライダー</param>
	void NotifyOnTriggerStay(BaseCollider* other);

	/// <summary>
	/// 離れた瞬間ということを各コンポーネントに通知する
	/// </summary>
	/// <param name="other">衝突対象のコライダー</param>
	void NotifyOnTriggerExit(BaseCollider* other);
private://メンバ関数ポインタの配列
	//OnCollisionを通知する関数をまとめる用の型
	using NotifyOnCollision = void (GameObject::*)(const CollisionInfo& info);
	//衝突判定のテーブル
	static std::vector<NotifyOnCollision> onCollisionTable;

	//OnTriggerを通知する関数をまとめる用の型
	using NotifyOnTrigger = void (GameObject::*)(BaseCollider* other);
	//衝突判定のテーブル
	static std::vector<NotifyOnTrigger> onTriggerTable;
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
	//現在取得しているシーン
	BaseScene* currentScene_ = nullptr;
};