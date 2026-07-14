#pragma once
#include "Context.h"
#include "Input.h"
#include "DebugCamera.h"
#include "Camera.h"
#include <memory>

// 前方宣言
class DirectXBase;
class DebugCamera;
class AbstractSceneFactory;
class ColliderManager;
class GameObject;

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
	/// <param name="sceneContext">シーンで必要なもの</param>
	virtual void Initialize(const SceneContext& sceneContext);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	virtual void Debug();

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	//純粋仮想関数
	virtual void Draw(Camera* camera) = 0;
	virtual void DebugDraw() = 0;
	virtual void GameDraw() = 0;

	/// <summary>
	/// ゲームオブジェクトの削除
	/// </summary>
	/// <param name="target">対象となるゲームオブジェクト</param>
	void DeleteGameObject(GameObject* target);

	/// <summary>
	/// ゲームオブジェトの複製
	/// </summary>
	/// <param name="target">対象となるゲームオブジェクト</param>
	void DuplicateGameObject(GameObject* target);

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
private://メンバ関数
	/// <summary>
	/// 名前を重複しないようにする
	/// </summary>
	/// <param name="baseName">元の名前</param>
	/// <param name="remove">省きたい部分</param>
	std::string CreateUniqueGameObjectName(const std::string& baseName, std::string_view remove)const;
private://定数
	//オブジェクトの大きさ
	static inline const uint32_t kGameObjectSize = 65536;
protected://メンバ変数
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
	//デバックカメラ
	std::unique_ptr<DebugCamera>debugCamera_ = nullptr;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	//コライダーマネージャー
	//std::unique_ptr<ColliderManager>colliderManager_ = nullptr;
	//ゲームプレイ用のカメラ
	Camera* gameCamera_ = nullptr;
	//ゲームオブジェクトの一覧
	std::vector<std::unique_ptr<GameObject>> gameObjects_;
};

