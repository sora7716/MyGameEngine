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
	void Initialize(LODBuilder* lodBuilder);

	/// <summary>
	/// 距離によってLODモデルの添え字を取得
	/// </summary>
	/// <param name="distance">距離</param>
	/// <param name="currentLOD">現在のLOD</param>
	/// <returns>LODモデルの添え字</returns>
	uint32_t SelectLOD(float distance, uint32_t currentLOD)const;

	/// <summary>
    /// LODの切り替え距離の設定
    /// </summary>
	/// <param name="lodDistances">LODの切り替え距離</param>
	void SetLODDistances(const std::vector<float>& lodDistances);
private://メンバ変数
	//LODビルダー
	LODBuilder* lodBuilder_ = nullptr;
	//LODの距離
	std::vector<float>lodDistances_;
};

