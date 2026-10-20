#pragma once
#include "LightingData.h"
#include "Component.h"
#include "PropertyRegistry.h"

/// <summary>
/// 平行光源
/// </summary>
class DirectionalLight:public Component{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit DirectionalLight(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DirectionalLight()override;

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
	virtual void RegisterProperties(PropertyRegistry* registry);

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;

	/// <summary>
	/// ライトの有効/無効の設定
	/// </summary>
	/// <param name="enabledLighting">ライトの有効/無効</param>
	void SetEnabledLighting(bool enabledLighting);

	/// <summary>
	/// ランバート反射モデルにするか
	/// </summary>
	/// <param name="isLambert">ランバート反射に切り替えるフラグ</param>
	void SetIsLambert(bool isLambert);

	/// <summary>
	/// Blinn-Phong反射に切り替えるか
	/// </summary>
	/// <param name="isBlinnPhong">Blinn-Phong反射に切り替えるフラグ</param>
	void SetIsBlinnPhong(bool isBlinnPhong);

	/// <summary>
	/// 光の強さを設定
	/// </summary>
	/// <param name="intensity">光の強さ</param>
	void SetIntensity(float intensity);

	/// <summary>
	/// 色を設定
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Color& color);

	/// <summary>
	/// 平行光源のデータを取得
	/// </summary>
	/// <returns>平行光源データ</returns>
	lightData::DirectionalLight GetDirectionalLightData()const;
private://メンバ変数
	//ライトの有効/無効
	bool enableLighting_ = true;
	//ライトの向き
	Vector3 direction_ = Vector3::GetZero();
	//ライトの色
	Color color_ = Color::GetWhite();
	//光の強さ
	float intensity_ = 1.0f;
	//ランバート反射に切り替えるか
	bool isLambert_ = false;
	//Blinn-Phong反射に切り替えるか
	bool isBlinnPhong_ = true;
};

