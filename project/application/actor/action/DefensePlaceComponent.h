#pragma once
#include "Component.h"
#include <string>
#include <memory>

/// <summary>
/// オブジェクトの設置
/// </summary>
class DefensePlaceComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit DefensePlaceComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DefensePlaceComponent()override;

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
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;

	/// <summary>
	/// 設置要求の設定
	/// </summary>
	/// <param name="confirmRequest">設置要求</param>
	void SetConfirmRequest(bool confirmRequest);

	/// <summary>
	/// 設置要求の取得
	/// </summary>
	/// <returns>設置要求</returns>
	bool GetConfirmRequest()const ;
private://メンバ変数
	//設置の要求
	bool confirmRequest_ = false;
};
