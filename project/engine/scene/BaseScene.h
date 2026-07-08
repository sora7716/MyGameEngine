#pragma once
#include "Context.h"
#include "Input.h"
#include "DebugCamera.h"
#include "Camera.h"
#include <memory>

// 前方宣言
class AbstractSceneFactory;
class DebugCamera;
class ColliderManager;
class DirectXBase;

/// <summary>
/// シーンの基底クラス
/// </summary>
class BaseScene {
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
	virtual void Draw() = 0;
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
	//描画用のカメラ
	Camera renderCamera_;
	//ゲームプレイ用のカメラ
	Camera* gameCamera_ = nullptr;
};

