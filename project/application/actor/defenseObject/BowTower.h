#pragma once
#include "BaseDefenseObject.h"

/// <summary>
/// 弓タワー
/// </summary>
class BowTower :public BaseDefenseObject{
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
private://メンバ関数
	/// <summary>
	/// 探しているときのアニメーション
	/// </summary>
	void UpdateSearchAnimation();
private://メンバ変数
	//頭の回転の最大角度(度)
	float maxHeadRotate_ = 60.0f;
	//頭の回転の最小角度(度)
	float minHeadRotate_ = -60.0f;
	float offset = {};
};