#pragma once
#include <memory>
//前方宣言
class GameObject;
class Object3d;

/// <summary>
/// アニメーションのインターフェース
/// </summary>
class IAnimation {
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IAnimation();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IAnimation();

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	virtual void Setup(GameObject* gameObject);

	/// <summary>
	/// 初期化
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
	virtual std::unique_ptr<IAnimation>Clone() = 0;
protected://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
};

