#pragma once
#include "engine/base/Context.h"
#include "engine/input/Input.h"
#include "engine/debug/DebugCamera.h"
#include <memory>

// 前方宣言
class AbstractSceneFactory;
class DebugCamera;
class ColliderManager;
class DirectXBase;

/// <summary>
/// シーンのインターフェース
/// </summary>
class IScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IScene();

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
	/// 終了
	/// </summary>
	virtual void Finalize();

	//純粋仮想関数
	virtual void Draw() = 0;
protected://メンバ変数
	//シーンで必要なもの
	SceneContext sceneContext_ = {};
	//デバックカメラ
	std::unique_ptr<DebugCamera>debugCamera_ = nullptr;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
	//コライダーマネージャー
	std::unique_ptr<ColliderManager>colliderManager_ = nullptr;
};

