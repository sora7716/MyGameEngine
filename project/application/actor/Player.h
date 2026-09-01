#pragma once
#include "Component.h"
#include "Vector3.h"

//前方宣言
class Input;

/// <summary>
/// プレイヤー
/// </summary>
class Player :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <param name="input">入力</param>
	explicit Player(GameObject* gameObject, Input& input);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;
private://定数
	static inline const float kSpeed = 0.5f;
private://メンバ変数
	//入力
	Input& input_;
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//速度
	Vector3 velocity_ = {};
};

