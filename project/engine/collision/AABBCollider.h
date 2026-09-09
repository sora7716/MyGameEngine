#pragma once
#include "PrimitiveData.h"
#include "BaseCollider.h"

/// <summary>
/// AABBのコライダー
/// </summary>
class AABBCollider :public BaseCollider{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit AABBCollider(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~AABBCollider()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// ハーフサイズの設定
	/// </summary>
	/// <param name="halfSize">ハーフサイズ</param>
	void SetHalfSize(const Vector3& halfSize);

	/// <summary>
	/// AABBの取得
	/// </summary>
	/// <returns>AABB</returns>
	const primitiveData::AABB& GetAABB();

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コライダータイプの取得
	/// </summary>
	/// <returns>コライダータイプ</returns>
	ColliderType GetColliderType()const override;
private://メンバ変数
	//AABB
	primitiveData::AABB aabb_ = {};
	//ハーフサイズ
	Vector3 halfSize_ = {};
};

