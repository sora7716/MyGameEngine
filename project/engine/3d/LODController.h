#pragma once
#include <vector>
//前方宣言
class LODBuilder;


/// <summary>
/// LODの制御
/// </summary>
class LODController{
public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="lodBuilder">LODビルダー</param>
	/// <param name="lodDistances">LODの距離</param>
	void Initialize(LODBuilder* lodBuilder, const std::vector<float>& lodDistances);

	/// <summary>
	/// 距離によってLODモデルの添え字を取得
	/// </summary>
	/// <param name="distance">距離</param>
	/// <param name="currentLOD">現在のLOD</param>
	/// <returns>LODモデルの添え字</returns>
	uint32_t SelectLOD(float distance, uint32_t currentLOD)const;
private://メンバ変数
	//LODビルダー
	LODBuilder* lodBuilder_ = nullptr;
	//LODの距離
	std::vector<float>lodDistances_;
};

