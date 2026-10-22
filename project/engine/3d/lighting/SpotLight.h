#pragma once
#include "Component.h"
#include "LightingData.h"

/// <summary>
/// スポットライト
/// </summary>
class SpotLight :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit SpotLight(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SpotLight()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// プロパティ一覧を登録
	/// </summary>
	/// <param name="registry">レジストリ</param>
	void RegisterProperties(PropertyRegistry* registry)override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;

	/// <summary>
	/// スポットライトのデータを取得
	/// </summary>
	/// <returns>スポットライトのデータ</returns>
	lightData::SpotLight GetSpotLightData()const;
private://メンバ変数
	Color color_ = Color::GetWhite();//ライト色
	float intensity_ = 1.0f;//輝度
	Vector3 direction_ = {};//スポットライトの方向
	float distance_ = 10.0f;//ライトの届く最大距離
	float decay_ = 2.0f;//減衰率
	float cosAngle_ = 0.7071f;//外接円
	float cosFalloffStart_ = 0.8660f;//内接円
	bool isBlinnPhong_ = true;//BlinnPhongReflectionを行うかどうか
	bool enableLighting_ = true;//点光源を有効にするか
};

