#pragma once
#include "BaseScene.h"
#include "ColliderData.h"
#include "LightingData.h"
#include "Input.h"
#include <vector>

//前方宣言
class Audio;
class ParticleSystem;

/// <summary>
/// テストプレイシーン
/// </summary>
class TestPlayScene :public BaseScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TestPlayScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TestPlayScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug()override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">使用するカメラ</param>
	void Draw(Camera*camera)override;

	/// <summary>
	/// デバッグでの描画
	/// </summary>
	void DebugDraw()override;

	/// <summary>
	/// ゲームでの描画
	/// </summary>
	void GameDraw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	DirectionalLight directionalLight_ = {};
};
