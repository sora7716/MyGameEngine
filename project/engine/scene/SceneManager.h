#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "CameraRenderData.h"
#include <memory>

//前方宣言
class DebugEditor;

/// <summary>
/// シーン管理
/// </summary>
class SceneManager{
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="sceneContext">シーンで必要なもの</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<SceneManager>Create(ConstructorKey key, const SceneContext& sceneContext);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit SceneManager(ConstructorKey);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager();

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
	/// 描画
	/// </summary>
	/// <param name="cameraMode">カメラモード</param>
	void Draw(CameraMode cameraMode);

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
private://メンバ関数
	//コピーコンストラクタを禁止
	SceneManager(const SceneManager&) = delete;
	//代入演算子を禁止
	SceneManager operator=(const SceneManager&) = delete;
private://メンバ変数
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	//シーン
	BaseScene* scene_ = nullptr;
	//次のシーン
	BaseScene* nextScene_ = nullptr;
	//デバッグエディタ
	std::unique_ptr<DebugEditor>debugEditor_ = nullptr;
};

