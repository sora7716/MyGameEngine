#pragma once
#include "BaseScene.h"
#include "LightingData.h"

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
	/// デバッグ
	/// </summary>
	void Debug()override;
private://メンバ変数
};
