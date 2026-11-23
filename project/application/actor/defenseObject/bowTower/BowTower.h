#pragma once
#include "BaseDefenseObject.h"
#include "PrimitiveData.h"
#include "Matrix4x4.h"

//前方宣言
class Object3d;

/// <summary>
/// 弓タワー
/// </summary>
class BowTower :public BaseDefenseObject {
public://列挙型
	//振る舞い
	enum class Behavior :uint32_t {
		kNone,
		kScan,
		kAttack,
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit BowTower(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BowTower()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	virtual void RegisterProperties(PropertyRegistry* registry);

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;

	/// <summary>
	/// 攻撃対象を設定
	/// </summary>
	/// <param name="target">対象</param>
	void SetTarget(GameObject* target);

	/// <summary>
	/// 矢を発射の要求を設定
	/// </summary>
	/// <param name="request">要求</param>
	void SetFireRequest(bool request);

	/// <summary>
	/// スキャンする半径の取得
	/// </summary>
	/// <returns>スキャンする半径</returns>
	float GetScanRadius()const;
	
	/// <summary>
	/// 弓を発射するかを取得
	/// </summary>
	/// <returns>発射するか</returns>
	bool IsFire()const;
private://メンバ関数
	/// <summary>
	/// 攻撃範囲を設定
	/// </summary>
	void SettingAttackRange();

	/// <summary>
	/// 探索範囲を設定
	/// </summary>
	void SettingScanRange();

	/// <summary>
	/// 探しているときのアニメーションの初期化
	/// </summary>
	void InitializeScanAnimation();

	/// <summary>
	/// 探しているときのアニメーションの更新
	/// </summary>
	void UpdateScanAnimation();

	/// <summary>
	/// 攻撃するときのアニメーションの初期化
	/// </summary>
	void InitializeAttackAnimation();

	/// <summary>
	///攻撃するときのアニメーションの更新
	/// </summary>
	void UpdateAttackAnimation();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
	//攻撃対象
	GameObject* target_ = nullptr;

	//振る舞いの要求
	Behavior behaviorRequest_ = Behavior::kScan;
	//振る舞い
	Behavior behavior_ = Behavior::kNone;

	//頭の位置を補完する速度
	float headTurnBlendSpeed_ = 12.0f;

	//矢を生成する要求
	bool fireRequest_ = false;

	//弓の攻撃範囲
	primitiveData::Sector attackRange_ = {};
	//攻撃範囲
	float attackRangeAngle_ = 30.0f;
	//攻撃時間を管理するタイマー
	float attackTimer_ = 0.0f;
	//攻撃のインターバル
	float attackInterval_ = 1.0f;
	//反動を管理するタイマー
	float recoilTimer_ = 0.0f;
	//反動の最大角度
	float recoilMaxDegrees = 20.0f;

	//探索範囲
	primitiveData::Sector scanRange_ = {};
	//探索範囲に入っているか
	bool isInScanRange_ = false;
	//探索をする最大角度(度)
	float maxScanRotate_ = 75.0f;
	//探索をする最小角度(度)
	float minScanRotate_ = -75.0f;
	//探索する際の回転オフセット
	float scanRotOffset_ = {};
	//探索する速度
	float scanSpeed_ = 2.0f;
	//探索している距離
	float scanRadius_ = 3.0f;
	//今探索している向き
	float scanDir_ = 1.0f;
	//スキャン開始位置
	Vector3 scanBeginRotate = {};
	//頭の行列
	Matrix4x4 headModelMatrix_ = {};
};