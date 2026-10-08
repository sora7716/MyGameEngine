#pragma once
#include "PrimitiveData.h"
#include "BaseCollider.h"

/// <summary>
/// Boxコライダー
/// </summary>
class BoxCollider :public BaseCollider{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit BoxCollider(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BoxCollider()override;

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
	/// オフセットを設定
	/// </summary>
	/// <param name="offset"></param>
	void SetOffset(const Vector3& offset);

	/// <summary>
	/// ハーフサイズの取得
	/// </summary>
	/// <returns>ハーフサイズ</returns>
	const Vector3& GetHalfSize()const;

	/// <summary>
	/// AABBの取得
	/// </summary>
	/// <returns>AABB</returns>
	const primitiveData::AABB& GetAABB();

	/// <summary>
	/// オフセットの取得
	/// </summary>
	/// <returns>オフセット</returns>
	const Vector3& GetOffset()const;

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
	//ワールドのハーフサイズ
	Vector3 worldHalfSize_ = {};
	//オフセット
	Vector3 offset_ = {};
};

