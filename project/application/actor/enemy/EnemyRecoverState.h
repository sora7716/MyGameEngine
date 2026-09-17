#pragma once
#include "IEnemyState.h"
#include "Quaternion.h"
#include "Vector3.h"

//前方宣言
class GameObject;
class RigidBody;

/// <summary>
/// 起き上がるとき
/// </summary>
class EnemyRecoverState :public IEnemyState{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyRecoverState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyRecoverState()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵</param>
	void Initialize(Enemy* enemy)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ関数
	/// <summary>
	/// 浮き上がるときの初期化
	/// </summary>
	void InitializeRecover();

	/// <summary>
	/// 浮き上がるときの更新
	/// </summary>
	void UpdateRecover();
private://定数
	//元に戻す
	static inline const float kRecoverDuration = 0.2f;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//リジットボディ
	RigidBody* rigidBody_ = nullptr;

	//衝突した方向ベクトル
	Vector3 hitDirection_ = {};

	//元に戻す
	Quaternion normalRotate_ = {};
	Quaternion recoverStartRotate_{};
	float recoverTimer_ = 0.0f;
};

