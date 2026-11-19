#pragma once
#include "Component.h"
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// 設置するオブジェクトの切り替え
/// </summary>
class PlacementSwitchComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit PlacementSwitchComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlacementSwitchComponent()override;


	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="modelNameList">モデル名リスト</param>
	void Setup(const std::vector<std::string>& modelNameList);

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
	/// 実行するか設定
	/// </summary>
	/// <param name="isRunning">実行するか</param>
	void SetIsRunning(bool isRunning);

	/// <summary>
	/// 切り替え要求を設定
	/// </summary>
	/// <param name="request">要求</param>
	void SetSwitchRequest(bool request);

	/// <summary>
	/// 切り替える方向を設定
	/// </summary>
	/// <param name="dir">切り替える方向</param>
	void SetSwitchDir(int32_t dir);

	/// <summary>
	/// 現在のモデル名を取得
	/// </summary>
	/// <returns>現在のモデル名</returns>
	const std::string& GetCurrentModelName()const;

	/// <summary>
	/// 切り替えたことを通知するフラグの取得
	/// </summary>
	/// <returns>切り替えたことを通知</returns>
	bool IsSwitched()const;
private://メンバ変数
	//実行するか
	bool isRunning_ = false;
	//動かす向き
	int32_t dir_ = 0;
	//現在のインデックス
	int32_t currentIndex_ = 0;
	//モデル名のリスト
	std::vector <std::string>modelNameList_;
	//切り替えの要求
	bool switchRequest_ = false;
	//切り替えたことを通知する
	bool isSwitched_ = false;
};

