#pragma once
#include "Component.h"
#include "Vector3.h"

//前方宣言
class Input;
class RigidBody;

/// <summary>
/// プレイヤー
/// </summary>
class Player :public Component{
public://列挙型
	//状態
	enum class Behavior :uint32_t{
		kRoot,
		kMove,
		kAttack,
		kCount
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Player(GameObject* gameObject);

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

	/// <summary>
	/// 衝突したら
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollisionStay(const CollisionInfo& info);

	/// <summary>
	/// カメラのオブジェクトの設定
	/// </summary>
	/// <param name="cameraObject">カメラのオブジェクト</param>
	void SetCameraObject(GameObject* cameraObject);
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
	/// 移動方向に向かせる
	/// </summary>
	void LookAt();

	/// <summary>
	/// 通常状態の初期化
	/// </summary>
	void RootInitialize();

	/// <summary>
	/// 通常状態の更新
	/// </summary>
	void RootUpdate();

	/// <summary>
	/// 移動状態の初期化
	/// </summary>
	void MoveInitialize();

	/// <summary>
	/// 移動状態の更新
	/// </summary>
	void MoveUpdate();

	/// <summary>
	/// 攻撃状態の初期化
	/// </summary>
	void AttackInitialize();

	/// <summary>
	/// 攻撃状態の更新
	/// </summary>
	void AttackUpdate();
private://定数
	//移動速度
	static inline const float kMoveSpeed = 10.0f;
	//移動の際のレスポンス
	static inline const float kMoveResponse = 12.0f;
	//ジャンプ速度
	static inline const float kJumpSpeed = 15.0f;
	//重力
	static inline const float kGravity = -30.0f;
	//向くスピード
	static inline const float kLookAtSpeed = 8.0f;
private://メンバ変数
	//入力
	Input* input_ = nullptr;
	//カメラオブジェクト
	GameObject* cameraObject_ = nullptr;
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;

	//リジットボディ
	RigidBody* rigidBody_ = nullptr;

	//入力されて移動方向ベクトル
	Vector3 inputDirection_ = {};
	//World座標系での移動方向ベクトル
	Vector3 worldDirection_ = {};

	//振る舞い
	Behavior behavior_ = Behavior::kCount;
	//振る舞いのリクエスト
	Behavior behaviorRequest_ = Behavior::kRoot;
};