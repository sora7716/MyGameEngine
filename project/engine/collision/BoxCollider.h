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
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry);

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
	/// OBBの取得
	/// </summary>
	/// <returns>OBB</returns>
	const primitiveData::OBB& GetOBB();

	/// <summary>
	/// AABBの取得
	/// </summary>
	/// <returns>AABB</returns>
	primitiveData::AABB GetAABB();

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 軸が平行かどうかを取得
	/// </summary>
	/// <returns>軸が平行かどうか</returns>
	bool IsAxisAligned();

	/// <summary>
	/// コンポーネントのタイプ名を取得
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;

	/// <summary>
	/// コライダータイプの取得
	/// </summary>
	/// <returns>コライダータイプ</returns>
	ColliderType GetColliderType()const override;
private://メンバ変数
	//OBB
	primitiveData::OBB obb_ = {};
	//ハーフサイズ
	Vector3 halfSize_ = {};
	//オフセット
	Vector3 offset_ = {};
};

