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
	/// ゲームオブジェクトの一覧を取得
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects()const;
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

