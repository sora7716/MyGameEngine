#pragma once
#include "BaseDefenseObject.h"
#include "PrimitiveData.h"
#include "Matrix4x4.h"
#include <vector>

//前方宣言
class Object3d;
class IState;
class ArrowSystem;

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
	/// セットアップ
	/// </summary>
	/// <param name="arrowSystem">矢の管理システム</param>
	void Setup(ArrowSystem* arrowSystem);

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
	/// <param name="targets">対象(配列)</param>
	void SetTargets(const std::vector<GameObject*>& targets);

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
	/// 攻撃範囲の角度を取得
	/// </summary>
	/// <returns>攻撃範囲の角度</returns>
	float GetAttackRangeDegrees()const;

	/// <summary>
	/// 頭のアニメーションのブレンド速度の取得
	/// </summary>
	/// <returns>頭のアニメーションのブレンド速度</returns>
	float GetHeadTurnBlendSpeed()const;

	/// <summary>
	/// 攻撃範囲の取得
	/// </summary>
	/// <returns>攻撃範囲</returns>
	const primitiveData::Sector& GetAttackRange()const;

	/// <summary>
	/// 攻撃対象の取得
	/// </summary>
	/// <returns>攻撃対象</returns>
	GameObject* GetAttackTarget()const;
private://メンバ関数
	/// <summary>
	/// 攻撃範囲を設定
	/// </summary>
	void SettingAttackRange();

	/// <summary>
	/// 探索範囲を設定
	/// </summary>
	void SettingScanRange();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
	//攻撃対象(配列)
	std::vector<GameObject*> attackTargets_;
	//攻撃対象
	GameObject* attackTarget_ = nullptr;
	//矢の管理システム
	ArrowSystem* arrowSystem_ = nullptr;

	//振る舞いの要求
	Behavior behaviorRequest_ = Behavior::kScan;
	//振る舞い
	Behavior behavior_ = Behavior::kNone;

	//ステートの配列
	std::vector<std::unique_ptr<IState>>states_;
	//現在のステート
	IState* currentState_ = nullptr;

	//頭の位置を補完する速度
	float headTurnBlendSpeed_ = 12.0f;

	//矢を生成する要求
	bool fireRequest_ = false;

	//弓の攻撃範囲
	primitiveData::Sector attackRange_ = {};
	//攻撃範囲の角度
	float attackRangeDegrees_ = 30.0f;

	//探索範囲
	primitiveData::Sector scanRange_ = {};
	//探索範囲に入っているか
	bool isInScanRange_ = false;
	//探索をする最大角度(度)
	float maxScanRotate_ = 75.0f;
	//探索している距離
	float scanRadius_ = 3.0f;
	//頭の行列
	Matrix4x4 headModelMatrix_ = {};
};