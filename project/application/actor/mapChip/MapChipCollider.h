#pragma once
#include "Component.h"
#include "MapChipData.h"

/// <summary>
/// マップチップのコライダー
/// </summary>
class MapChipCollider :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit MapChipCollider(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MapChipCollider()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="mapChip">マップチップのデータ</param>
	void Setup(const MapChipData& mapChip);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

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
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//マップのデータ
	MapChipData mapChip_ = {};
};

