#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sceneContext">シーンで必要なもの</param>
	void Initialize(const SceneContext& sceneContext);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// ゲーム画面の描画
	/// </summary>
	void GameDraw();

	/// <summary>
	/// デバッグ画面の描画
	/// </summary>
	void DebugDraw();

	/// <summary>
	/// シーンファクトリーのセッター
	/// </summary>
	/// <param name="sceneFactory">シーンファクトリー</param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory);

	/// <summary>
	/// シーン切り替え
	/// </summary>
	/// <param name="sceneName"></param>
	void ChangeScene(const std::string& sceneName);
public://PassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit SceneManager(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタを禁止
	SceneManager(const SceneManager&) = delete;
	//代入演算子を禁止
	SceneManager operator=(const SceneManager&) = delete;
private://静的メンバ変数
	//インスタンス
	static inline SceneManager* instance = nullptr;
	//Finalizeをしたかどうか
	static inline bool isFinalize = false;
private://メンバ変数
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	//シーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;
};

