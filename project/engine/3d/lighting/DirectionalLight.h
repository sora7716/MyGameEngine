#pragma once
#include "LightingData.h"
#include "Component.h"

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
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;
private://メンバ変数
	//平行光源
	lightData::DirectionalLight directionalLight_ = {};
};

