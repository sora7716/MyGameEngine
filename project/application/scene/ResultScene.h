#pragma once
#include "BaseScene.h"
#include "Vector2.h"

//前方宣言
class Text;
class StageTimer;

/// <summary>
/// タイトルシーン
/// </summary>
class ResultScene :public BaseScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ResultScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ResultScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="sceneContext">シーンで必要なもの</param>
	void Initialize(const SceneContext& sceneContext)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize()override;
private://メンバ変数
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//スコア
	std::unique_ptr<StageTimer>stageTimer_ = nullptr;
	//スコアの表示
	std::unique_ptr<Text>drawScore_ = nullptr;
	Vector2 scorePos_ = { 60.0f,300.0f };
	Vector2 scoreScale_ = { 500.0f,500.0f };
	float scoreTextSize_ = 64.0f;

	//リターン
	std::unique_ptr<Text>pressReturn_ = nullptr;
	Vector2 pressReturnPos_ = {250.0f,600.0f};
	float pressReturnSize_ = 64.0f;
};
