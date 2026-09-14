#pragma once
#include "Component.h"
#include <GameObject.h>

//前方宣言
class BaseCollider;

//衝突判定の情報
struct CollisionInfo{
	//衝突した対象
	BaseCollider* other = nullptr;
	//押し戻すベクトル
	Vector3 normal = {};
	//めり込んだ距離
	float penetrationDepth = 0.0f;
};

//コライダータイプ
enum class ColliderType :uint32_t{
	kAABB,
	kSphere,
	kOBB
};

//ボディタイプ
enum class BodyType :uint32_t{
	kStatic,//動かせない
	kDynamic//動かせる
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
	/// 衝突したときの判定(押し戻しあり)
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollisionStay(const CollisionInfo& info)override;

	/// <summary>
	/// 衝突したときの判定(押し戻しなし)
	/// </summary>
	/// <param name="other">コライダーの情報</param>
	void OnTriggerStay(BaseCollider* other)override;

	/// <summary>
	/// めり込むかを判定するフラグの設定
	/// </summary>
	/// <param name="isTrigger">めり込むかを判定する</param>
	void SetIsTrigger(bool isTrigger);

	/// <summary>
	/// ボディタイプの設定
	/// </summary>
	/// <param name="bodyType">ボディタイプ</param>
	void SetBodyType(BodyType bodyType);

	/// <summary>
	/// めり込むかを判定するフラグを取得
	/// </summary>
	/// <returns>めり込むかを判定する</returns>
	bool IsTrigger()const;

	/// <summary>
	/// ボディタイプの取得
	/// </summary>
	/// <returns>ボディタイプ</returns>
	BodyType GetBodyType()const;

	/// <summary>
	/// コライダータイプの取得
	/// </summary>
	/// <returns>コライダータイプ　</returns>
	virtual ColliderType GetColliderType()const = 0;
protected://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
private://メンバ変数
	//めり込むかを判定するフラグ
	bool isTrigger_ = false;
	//BodyTye
	BodyType bodyType_ = BodyType::kStatic;
};

