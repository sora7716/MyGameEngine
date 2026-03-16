#pragma once
#include "TextData.h"
#include "RenderingData.h"
#include <memory>

//前方宣言
class Text;
class Object2dCommon;

/// <summary>
/// ゲームステージのタイマー
/// </summary>
class StageTimer{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StageTimer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StageTimer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object2dCommon">2Dの共通部分</param>
	void Initialize(Object2dCommon*object2dCommon);

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
	void Draw();

	/// <summary>
	/// 時間切れのフラグのゲッター
	/// </summary>
	/// <returns>時間切れ</returns>
	bool IsTimeUp();
private://メンバ変数
	//テキスト
	std::unique_ptr<Text> text_ = nullptr;
	TextStyle textStyle_ = {};
	//トランスフォーム
	Transform2dData transformData_ = {};
	//タイマー
	float timer_ = 100.0f;
	//時間切れ
	bool isTimeUp_ = false;
};