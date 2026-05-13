#pragma once
#include "BaseScene.h"
#include "Vector2.h"
class Text;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene :public BaseScene {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene()override;

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
private://メンバ変数n  
	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;
	//ゲームタイトル
	std::unique_ptr<Text>titleName_ = nullptr;
	Vector2 titleNamePos_ = {};
	float titleNameSize_ = 128.0f;

	//スタートの開始ボタン
	std::unique_ptr<Text>pressStart_=nullptr;
	Vector2 pressStartPos_ = {};
	float pressStartSize_ = 64.0f;
};
