#pragma once
#include "Context.h"
#include "Input.h"
#include "Camera.h"
#include <memory>

// 前方宣言
class DirectXBase;
class AbstractSceneFactory;
class GameObject;
class DebugCameraController;
class Camera;
class CollisionSystem;

/// <summary>
/// シーンの基底クラス
/// </summary>
class BaseScene{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseScene();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 更新のステート
	/// </summary>
	virtual void UpdateState();

	/// <summary>
	/// デバッグ
	/// </summary>
	virtual void Debug();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// ゲームオブジェクトの削除
	/// </summary>
	/// <param name="target">対象となるゲームオブジェクト</param>
	/// <param name="collisionSystem">衝突判定のシステム</param>
	void DeleteGameObject(GameObject* target,CollisionSystem*collisionSystem);

	/// <summary>
	/// ゲームオブジェトの複製
	/// </summary>
	/// <param name="target">対象となるゲームオブジェクト</param>
	void DuplicateGameObject(GameObject* target);

	/// <summary>
	/// ゲームオブジェクトの位置(配列の順番)の変更
	/// </summary>
	/// <param name="from">今いる場所</param>
	/// <param name="to">最終的に置いておく場所</param>
	void MoveGameObject(uint32_t from, uint32_t to);

	/// <summary>
	/// ゲームオブジェクトのタグを古いのから新しいのに変更
	/// </summary>
	/// <param name="oldTag">古い名前</param>
	/// <param name="newTag">新しい名前</param>
	void ReplaceGameObjectTag(const std::string& oldTag, const std::string& newTag);

	/// <summary>
	/// 空のゲームオブジェクトを生成
	/// </summary>
	/// <returns>空のゲームオブジェクト</returns>
	GameObject* CreateGameObject();

	/// <summary>
	/// ゲームオブジェクトの一覧を取得
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects()const;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="sceneContext">シーンに必要な情報</param>
	void SetUp(const SceneContext& sceneContext);

	/// <summary>
	/// シーンで必要な情報の取得
	/// </summary>
	/// <returns>シーンに必要な情報</returns>
	const SceneContext& GetSceneContext();

	/// <summary>
	/// デバッグが有効かの設定
	/// </summary>
	/// <param name="isControlEnabled">デバッグが有効か</param>
	void SetIsDebugControlEnabled(bool isControlEnabled);

	/// <summary>
	/// アプリケーションが有効かの設定
	/// </summary>
	/// <param name="isAppInputEnabled">アプリケーションが有効か</param>
	void SetIsAppInputEnabled(bool isAppInputEnabled);

	/// <summary>
	/// デバッグカメラの取得
	/// </summary>
	/// <returns>デバッグカメラ</returns>
	Camera* GetDebugCamera()const;
private://メンバ関数
	/// <summary>
	/// 名前を重複しないようにする
	/// </summary>
	/// <param name="baseName">元の名前</param>
	/// <param name="remove">省きたい部分</param>
	std::string CreateUniqueGameObjectName(const std::string& baseName, std::string_view remove)const;
private://定数
	//オブジェクトのメモリ確保数
	static inline const uint32_t kGameObjectSize = 65536;
protected://メンバ変数
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
private://メンバ変数
	//シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
	//ゲームオブジェクトの一覧
	std::vector<std::unique_ptr<GameObject>> gameObjects_;
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
	//デバッグカメラの操作
	DebugCameraController* debugCameraController_ = nullptr;
	//デバッグカメラのポインタ
	Camera* debugCamera_ = nullptr;
};