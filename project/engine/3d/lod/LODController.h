#pragma once
#include <cstdint>
#include <vector>

/// <summary>
/// LODの制御
/// </summary>
class LODController {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LODController();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LODController();

	/// <summary>
	/// ヒステリシス幅の計算
	/// </summary>
	void CalculateHysteresisWidth();

	/// <summary>
	/// 距離によってLODモデルの添え字を取得
	/// </summary>
	/// <param name="distance">距離</param>
	/// <param name="currentLOD">現在のLOD</param>
	/// <param name="lodCount">LODの数</param>
	/// <returns>LODモデルの添え字</returns>
	uint32_t SelectLOD(float distance,uint32_t currentLOD,uint32_t lodCount)const;

	/// <summary>
	/// LODの切り替え距離の設定
	/// </summary>
	/// <param name="lodDistances">LODの切り替え距離</param>
	void SetLODDistances(const std::vector<float>& lodDistances);
private://メンバ変数
	//LODの距離
	std::vector<float>lodDistances_;
	//ヒステリシス幅
	float hysteresis_ = 0.0f;
};

