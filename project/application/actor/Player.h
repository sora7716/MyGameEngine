#pragma once
#include "Component.h"
#include "Vector3.h"
#include "RenderingData.h"

//前方宣言
class Input;
class RigidBody;
class Object3d;

/// <summary>
/// プレイヤー
/// </summary>
class Player :public Component{
public://列挙型
	//状態
	enum class Behavior :uint32_t{
		kNormal,
		kMove,
		kAttack,
		kCount
	};

	//プレイヤーのTransform
	struct PlayerPose{
		Transform root = {};
		Transform head = {};
		Transform body = {};
		Transform leftArm = {};
		Transform rightArm = {};

		/// <summary>
		/// 補間
		/// </summary>
		/// <param name="playerPose1">プレイヤーポーズ</param>
		/// <param name="playerPose2">プレイヤーポーズ</param>
		/// <param name="t">係数</param>
		/// <returns>補間したプレイヤーポーズ</returns>
		static PlayerPose Lerp(const PlayerPose& playerPose1, const PlayerPose& playerPose2, float t);
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
	/// デバッグでImGuiを使用できるようにする
	/// </summary>
	void DebugImGui()override;

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
	/// モーション遷移の初期化
	/// </summary>
	void InitializeTransition();

	/// <summary>
	/// モーション遷移の更新
	/// </summary>
	/// <param name="targetPose">目的のポーズ</param>
	void UpdateTransition(PlayerPose targetPose);

	/// <summary>
	/// 通常状態の初期化
	/// </summary>
	void InitializeNormal();

	/// <summary>
	/// 通常状態の更新
	/// </summary>
	void UpdateNormal();

	/// <summary>
	/// 移動状態の初期化
	/// </summary>
	void InitializeMoving();

	/// <summary>
	/// 移動状態の更新
	/// </summary>
	void UpdateMoving();

	/// <summary>
	/// 攻撃状態の初期化
	/// </summary>
	void InitializeAttack();

	/// <summary>
	/// 攻撃状態の更新
	/// </summary>
	void UpdateAttack();
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
	//モーションの切り替え時間
	static inline const float kTransitionDuration = 1.0f;
private://メンバ変数
	//入力
	Input* input_ = nullptr;
	//カメラオブジェクト
	GameObject* cameraObject_ = nullptr;
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//オブジェクト3d
	Object3d* object3d_ = nullptr;

	//リジットボディ
	RigidBody* rigidBody_ = nullptr;

	//入力されて移動方向ベクトル
	Vector3 inputDirection_ = {};
	//World座標系での移動方向ベクトル
	Vector3 worldDirection_ = {};

	//振る舞い
	Behavior behavior_ = Behavior::kCount;
	//振る舞いのリクエスト
	Behavior behaviorRequest_ = Behavior::kNormal;

	//今のNodeのLocalTransform情報
	PlayerPose currentPose_ = {};
	//前のNodeのLocalTransform情報
	PlayerPose prePose_ = {};

	//モーション切り替え用のタイマー
	float transitionTimer_ = 0.0f;

	//通常状態
	float bobTimer_ = 0.0f;
	//全体
	float bobRootAmplitude_ = 0.15f;
	float bobRootSpeed_ = 5.0f;
	//体
	float bobBodyAmplitude_ = 0.1f;
	float bobBodySpeed_ = 5.0f;
	//両腕
	float bobArmAmplitude_ = 0.3f;
	float bobArmSpeed_ = 5.0f;

	//移動状態
	float movingTimer_ = 0.0f;
	//全体
	float movingRootAmplitude_ = 0.4f;
	float movingRootSpeed_ = 5.0f;
	//両腕
	float movingArmAmplitude_ = 0.8f;
	float movingArmSpeed_ = 5.0f;

	//攻撃状態
};