#pragma once
#include "Component.h"
#include "EnemyData.h"

/// <summary>
/// 敵
/// </summary>
class Enemy :public Component {
private://列挙型
	//振る舞い
	enum class Behavior {
		kRoot,
		kShock,
		kDead,
		kCount,
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
	/// デバッグでImGuiを使えるようにする
	/// </summary>
	void DebugImGui()override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry)override;

	/// <summary>
	/// 接触した瞬間
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollisionEnter(const CollisionInfo& info)override;

	/// <summary>
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other)override;

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
	/// ショックパラメータを設定
	/// </summary>
	/// <param name="parameter">パラメータ</param>
	void SetShockParameter(const ShockParameter& parameter);
private://メンバ変数
	/// <summary>
	/// ステータスが存在しいるか取得
	/// </summary>
	/// <param name="status">存在してほしいステータス</param>
	/// <returns>ステータスが存在するか</returns>
	bool HasStatus(EnemyStatus status);

	/// <summary>
	/// 死亡処理
	/// </summary>
	void OnDead();

	/// <summary>
	/// ショック状態の初期化
	/// </summary>
	void InitializeShockState();

	/// <summary>
	/// ショック状態の更新
	/// </summary>
	void UpdateShockState();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;

	//振る舞いの要求
	Behavior behaviorRequest_ = Behavior::kRoot;
	//振る舞い
	Behavior behavior_ = Behavior::kCount;

	//敵の攻撃力
	float attackPower_ = 20.0f;

	//現在のステータス
	EnemyStatus status_ = EnemyStatus::kNone;

	//ショックパラメータ
	ShockParameter shockParameter_ = {};
	//ショックステート
	ShockState shockState_ = {};
};

