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
	/// スキャンする半径の取得
	/// </summary>
	/// <returns>スキャンする半径</returns>
	float GetScanRadius()const;
private://メンバ関数
	/// <summary>
	/// 探しているときのアニメーションの初期化
	/// </summary>
	void InitializeScanAnimation();

	/// <summary>
	/// 探しているときのアニメーションの更新
	/// </summary>
	void UpdateScanAnimation();
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//Object3d
	Object3d* object3d_ = nullptr;
	//攻撃対象
	GameObject* target_ = nullptr;
	bool flag_ = false;

	//弓の探索範囲
	primitiveData::Sector scanRange_ = {};
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