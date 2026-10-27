#pragma once
#include "PrimitiveData.h"
#include "BaseCollider.h"
#include "Vector3.h"

/// <summary>
/// Sphereコライダー
/// </summary>
class SphereCollider :public BaseCollider{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit SphereCollider(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SphereCollider()override;

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
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

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

	/// <summary>
	/// 球の取得
	/// </summary>
	/// <returns>球のデータ</returns>
	const primitiveData::Sphere& GetSphere()const;
private://メンバ変数
	//球
	primitiveData::Sphere sphere_ = {};
	//中心座標
	Vector3 center_ = {};
	//半径
	float radius_ = 0.0f;
};

