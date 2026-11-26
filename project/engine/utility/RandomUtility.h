#pragma once
#include <random>

/// <summary>
/// ランダムのユーティリティ
/// </summary>
class RandomUtility {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RandomUtility();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RandomUtility();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ランダムな値を取得
	/// </summary>
	/// <typeparam name="T">型</typeparam>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>ランダムな値(最小値~最大値の範囲で)</returns>
	template <typename T>
	T GetRandomValue(T min,T max) {
		std::uniform_real_distribution<T>distRandomValue(min,max);
		return distRandomValue;
	}
private://メンバ変数
	//ランダムエンジン
	std::mt19937 randomEngine_;
};

