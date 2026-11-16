#pragma once
#include <chrono>
class FixFPS{
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns>インスタンス</returns>
	static FixFPS* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	///更新
	/// </summary>
	void Update();

	/// <summary>
	/// FPSの取得
	/// </summary>
	/// <returns>FPS</returns>
	float GetFPS()const;

	//コピーコンストラクタ禁止
	FixFPS(const FixFPS&) = delete;
	//代入演算子を禁止
	const FixFPS& operator=(const FixFPS&) = delete;
private://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	FixFPS() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FixFPS() = default;
private://メンバ変数
	std::chrono::steady_clock::time_point reference_;//記録時間(FPS固定用)
	//現在のFPS
	float fps_ = 0.0f;
};

