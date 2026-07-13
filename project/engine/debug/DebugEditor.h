#pragma once
#include <vector>
#include <memory>
//前方宣言
class GameObject;

/// <summary>
/// デバッグエディター
/// </summary>
class DebugEditor{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugEditor();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugEditor();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ゲームオブジェクト一覧の設定
	/// </summary>
	/// <param name="gameObjects">ゲームオブジェクト一覧</param>
	void SetGameObjects(const std::vector<std::unique_ptr<GameObject>>& gameObjects);

	/// <summary>
	/// 削除要求を取得
	/// </summary>
	/// <returns>削除要求</returns>
	GameObject* ConsumeDeleteRequest();
private://メンバ関数
	/// <summary>
	/// ドッキングスペースの描画
	/// </summary>
	void DrawDockSpace();

	/// <summary>
	/// ヒエラルキーの描画
	/// </summary>
	void DrawHierarchy();

	/// <summary>
	/// インスペクターの描画
	/// </summary>
	void DrawInspector();
private://メンバ変数
	//選択するゲームオブジェクト
	GameObject* selectedGameObject_ = nullptr;
	//ゲームオブジェクトの一覧へのポインタ
	const std::vector<std::unique_ptr<GameObject>>*gameObjects_;
	//作成要求
	bool requestCreateGameObject_ = false;
	//複製要求
	GameObject* requestDuplicateGameObject_ = nullptr;
	//削除要求
	GameObject* requestDeleteGameObject_ = nullptr;
};

