#pragma once
#include "Component.h"
#include "Vector3.h"

//前方宣言
class Object3d;

/// <summary>
/// インディケータ
/// </summary>
class Indicator :public Component {
public://列挙型
	//振る舞い
	enum Behavior {
		kNone,
		kVisibility,//表示
		kHidden//非表示
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit Indicator(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Indicator()override;

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
	void RegisterProperties(PropertyRegistry* registry);

	/// <summary>
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other);

	/// <summary>
	/// 離れた瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerExit(BaseCollider* other);

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
	/// 最終的な目標の半径
	/// </summary>
	/// <param name="radius">半径</param>
	void SetShowTargetRadius(float radius);

	/// <summary>
	/// 見た目だけのゲームオブジェクトを設定
	/// </summary>
	/// <param name="visual"></param>
	void SetVisualObject(GameObject* visual);
private://メンバ関数
	/// <summary>
	/// 表示用のアニメーション
	/// </summary>
	void VisibilityAnimation();

	/// <summary>
	/// 非表示用のアニメーション
	/// </summary>
	void HiddenAnimation();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//見た目だけのゲームオブジェクト
	GameObject* visual_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
	//振る舞いの要求
	Behavior behaviorRequest_ = Behavior::kHidden;
	//振る舞い
	Behavior behavior_ = Behavior::kNone;
	//衝突して瞬間のScale
	Vector3 scaleAtCollision_ = {};
	//衝突範囲から抜けた瞬間のScale
	Vector3 scaleAtCollisionExit_ = {};
	//表示時の最終的な半径
	float showTargetRadius_ = 0.0f;
	//現在の経過時間(秒)
	float currentSecond_ = 0.0f;
	//終わる時間(秒)
	float endSecond_ = 0.4f;
};

