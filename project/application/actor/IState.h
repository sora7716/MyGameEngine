#pragma once
#include <memory>
//前方宣言
class GameObject;
class Object3d;

/// <summary>
/// ステートのインターフェース
/// </summary>
class IState {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IState();

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	virtual void Setup(GameObject* gameObject);

	/// <summary>
	/// 入った瞬間
	/// </summary>
	virtual void Enter() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Exit() = 0;

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns>複製</returns>
	virtual std::unique_ptr<IState>Clone() = 0;
protected://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
};

