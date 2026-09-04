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
private://メンバ関数
	/// <summary>
	/// 移動の操作
	/// </summary>
	void MoveControl();

	/// <summary>
	/// ジャンプの操作
	/// </summary>
	void JumpControl();

	/// <summary>
	/// 重力を適応
	/// </summary>
	void ApplyGravity();

	/// <summary>
	/// 速度を位置へ反映する
	/// </summary>
	void Movement();

	/// <summary>
	/// 地面との接触
	/// </summary>
	void ResolveGround();
private://定数
	//移動速度
	static inline const float kMoveSpeed = 10.0f;
	//移動の際のレスポンス
	static inline const float kMoveResponse = 12.0f;
	//ジャンプ速度
	static inline const float kJumpSpeed = 15.0f;
	//重力
	static inline const float kGravity = -30.0f;
private://メンバ変数
	//入力
	Input& input_;
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//加速度
	Vector3 acceleration_ = {};
	//速度
	Vector3 velocity_ = {};
	//地面の上にいるか
	bool isOnGround_ = true;
};