#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <array>

//前方宣言
class RigidBody;
class IEnemyState;

/// <summary>
/// 敵
/// </summary>
class Enemy :public Component{
public://構造体やenum
	//状態
	enum class Behavior :uint32_t{
		kRoot,
		kDamage,
		kRecover,
		kCount
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Enemy(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 衝突した瞬間
	/// </summary>
	/// <param name="other">衝突対象</param>
	void OnTriggerEnter(BaseCollider* other)override;

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 振る舞いのリクエストの設定
	/// </summary>
	/// <param name="request">リクエスト</param>
	void SetBehaviorRequest(Behavior request);

	/// <summary>
	/// リジットボディの取得
	/// </summary>
	/// <returns>リジットボディ</returns>
	RigidBody* GetRigidBody();

	/// <summary>
	/// 衝突した方向ベクトルの取得
	/// </summary>
	/// <returns>衝突した方向ベクトルの取得</returns>
	const Vector3& GetHitDirection()const;

	/// <summary>
	/// ダメージを受けた瞬間の回転を取得
	/// </summary>
	/// <returns>ダメージを受けた瞬間の回転</returns>
	const Quaternion& GetNormalRotate()const;
private://メンバ関数
	/// <summary>
	/// ステートの切り替え
	/// </summary>
	/// <param name="behavior">振る舞い</param>
	void ChangeState(Behavior behavior);
private://定数

private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;

	//リジッドボディ
	RigidBody* rigidBody_ = nullptr;

	//敵の状態
	Behavior behavior_ = Behavior::kCount;
	//敵の状態のリクエスト
	Behavior behaviorRequest_ = Behavior::kRoot;

	//ステート
	std::array < std::unique_ptr<IEnemyState>, static_cast<uint32_t>(Behavior::kCount)> states_;
	//現在のステート
	IEnemyState* currentState_ = nullptr;

	//衝突した方向
	Vector3 hitDirection_ = {};

	//通常状態の回転
	Quaternion normalRotate_ = {};
};

