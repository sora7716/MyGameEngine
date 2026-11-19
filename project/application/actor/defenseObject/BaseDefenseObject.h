#pragma once
#include "Component.h"
#include <cstdint>

/// <summary>
/// ディフェンス用のオブジェクト
/// </summary>
class BaseDefenseObject :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit BaseDefenseObject(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BaseDefenseObject()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 接触した瞬間
	/// </summary>
	/// <param name="other">コライダー</param>
	void OnTriggerEnter(BaseCollider* other)override;

	/// <summary>
	/// 離れた瞬間
	/// </summary>
	/// <param name="other"></param>
	void OnTriggerExit(BaseCollider* other)override;

	/// <summary>
	/// コンポーネントのタイプ名を返す
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const;

	/// <summary>
	/// 設置物と重なっているを取得
	/// </summary>
	/// <returns>重なっていたら</returns>
	bool IsOverlappingDefense()const;
private://メンバ変数
	//重なっているかのカウント
	uint32_t overlapCount_ = 0;
};

