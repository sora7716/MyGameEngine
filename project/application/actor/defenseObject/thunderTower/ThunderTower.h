#pragma once
#include "BaseDefenseObject.h"
#include <vector>

//前方宣言
class Object3d;
class MaterialInstance;

/// <summary>
/// 雷のタワー
/// </summary>
class ThunderTower :public BaseDefenseObject {
private://列挙型
	//チャージパーツ
	enum class ChargePart :uint32_t {
		kBottom,
		kMiddle,
		kTop,
		kCount
	};

	//振る舞い
	enum class Behavior :uint32_t {
		kIdol,
		kAttack,
		kCount
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit ThunderTower(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ThunderTower()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// デバッグでImGuiを使えるようにする
	/// </summary>
	void DebugImGui()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other)override;

	/// <summary>
	/// 接触中
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerStay(BaseCollider* other)override;

	/// <summary>
	/// 離れた瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerExit(BaseCollider* other)override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry)override;

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
private://メンバ関数
	/// <summary>
	/// 攻撃の初期化
	/// </summary>
	void InitializeAttack();

	/// <summary>
	/// 攻撃の更新
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// 落雷の生成
	/// </summary>
	/// <param name="spawnPos">スポーンする位置</param>
	void CreateLightningStrike(const Vector3& spawnPos);
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
	//振る舞いの要求
	Behavior behaviorRequest_ = Behavior::kIdol;
	//振る舞い
	Behavior behavior_ = Behavior::kCount;

	//デフォルトのマテリアル
	std::shared_ptr<MaterialInstance>defaultMat_ = nullptr;
	//フラッシュのマテリアル
	std::shared_ptr<MaterialInstance>flashMat_ = nullptr;

	//攻撃の計測タイマー
	float attackTimer_ = 0.0f;
	//攻撃のインターバル
	float attackInterval_ = 0.5f;

	//攻撃対象
	std::vector<GameObject*> targets_ = {};
};
