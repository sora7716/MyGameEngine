#pragma once
#include "Component.h"
#include "Vector3.h"
#include <string>
#include <vector>

//前方宣言
class Object3d;

/// <summary>
/// 設置予定地の表示/非表示n
/// </summary>
class PlacementPreviewComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit PlacementPreviewComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlacementPreviewComponent()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="targetObject3d">設置対象のObject3d</param>
	/// <param name="modelList">モデル一覧</param>
	void Setup(Object3d* targetObject3d,const std::vector<std::string>& modelList);

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
	/// モデルの検索キーの設定
	/// </summary>
	/// <param name="modelIndex">モデルの検索キー</param>
	void SetModelIndex(uint32_t modelIndex);

	/// <summary>
	/// 表示するかの要求を設定を設定
	/// </summary>
	/// <param name="request">要求</param>
	void SetEnabledRequest(bool request);

	/// <summary>
	/// 位置の設定
	/// </summary>
	/// <param name="position">位置</param>
	void SetPosition(const Vector3& position);

	/// <summary>
	/// 表示するかの要求を取得
	/// </summary>
	/// <returns>要求</returns>
	bool GetEnabledRequest()const;
private://メンバ変数
	//モデル名
	std::string modelName_ = "";
	//置く対象のObject3d
	Object3d* targetObject3d_ = nullptr;
	//モデル一覧
	std::vector<std::string>modelList_;
	//現在のモデルの検索キー
	uint32_t currentModelIndex_ = 0;
	//前回のモデルの検索キー
	uint32_t prevModelIndex_ = 0;
	//表示を要求
	bool enableRequest_ = false;
	//表示位置を設定
	Vector3 position_ = {};
};

