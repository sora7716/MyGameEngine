#pragma once
#include "LightingData.h"
#include "Component.h"
#include "PropertyRegistry.h"

/// <summary>
/// 点光源
/// </summary>
class PointLight :public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit PointLight(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PointLight()override;

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
	/// 点光源のデータを取得
	/// </summary>
	/// <returns>点光源のデータ</returns>
	lightData::PointLight GetPointLightData()const;
private://メンバ変数
	Color color_ = Color::GetWhite();//ライトの色
	float intensity_ = 1.0f;//輝度
	float distance_ = 1.0f;//ライトの届く最大距離
	float decay_ = 1.0f;//減衰率
	bool isBlinnPhong_ = true;//BlinnPhongReflectionを行うかどうか
	bool enableLighting_ = true;//点光源を有効にするか
};

