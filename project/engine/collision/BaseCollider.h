#pragma once
#include "Component.h"
#include <GameObject.h>

//衝突判定の情報
struct CollisionInfo{
	//衝突した対象
	BaseCollider* other = nullptr;
	//押し戻すベクトル
	Vector3 normal = {};
	//めり込んだ距離
	float penetrationDepth = 0.0f;
};

/// <summary>
/// コライダーの基底クラス
/// </summary>
class BaseCollider :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit BaseCollider(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BaseCollider()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 衝突したときの判定
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info);

	/// <summary>
	/// めり込むかを判定するフラグの設定
	/// </summary>
	/// <param name="isTrigger">めり込むかを判定する</param>
	void SetIsTrigger(bool isTrigger);

	/// <summary>
	/// めり込むかを判定するフラグを取得
	/// </summary>
	/// <returns>めり込むかを判定する</returns>
	bool IsTrigger();
protected://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
private://メンバ変数
	//めり込むかを判定するフラグ
	bool isTrigger_ = false;
};

