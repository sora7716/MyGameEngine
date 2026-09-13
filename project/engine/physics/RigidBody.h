#pragma once
#include "Component.h"
#include "Vector3.h"

/// <summary>
/// 物理演算をするためのボディ
/// </summary>
class RigidBody :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit RigidBody(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RigidBody()override;

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
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 速度の取得
	/// </summary>
	/// <returns>速度</returns>
	Vector3& GetVelocity();

	/// <summary>
	/// 速度の取得
	/// </summary>
	/// <returns>速度</returns>
	const Vector3& GetVelocity()const;

	/// <summary>
	/// 加速度の取得
	/// </summary>
	/// <returns>加速度</returns>
	Vector3& GetAcceleration();

	/// <summary>
	/// 加速度の取得
	/// </summary>
	/// <returns>加速度</returns>
	const Vector3& GetAcceleration()const;
private://メンバ関数
	/// <summary>
	/// 速度と加速度を適応
	/// </summary>
	void UpdateMotion();

	/// <summary>
	/// 重力を適応
	/// </summary>
	void ApplyGravity();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//速度
	Vector3 velocity_ = {};
	//加速度
	Vector3 acceleration_ = {};
	//重さ
	float mass = 1.0f;
};