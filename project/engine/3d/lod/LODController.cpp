#include "LODController.h"
#include <algorithm>
#include <cmath>

//コンストラクタ
LODController::LODController() {
}

//デストラクタ
LODController::~LODController() {
}

//ヒステリシス幅の計算
void LODController::CalculateHysteresisWidth() {
	//ヒステリシス幅の計算
	float minGap = lodDistances_[0];//一番小さい間隔
	//昇順にソート
	std::sort(lodDistances_.begin(),lodDistances_.end());
	for(uint32_t i = 0; i < lodDistances_.size() - 1; i++) {
		//間隔を取得
		float gap = std::fabs(lodDistances_[i] - lodDistances_[i + 1]);
		//最小値を設定
		if(minGap > gap) {
			minGap = gap;
		}
	}
	//ヒステリシス幅を設定
	constexpr float kHalf = 0.5f;
	hysteresis_ = minGap * kHalf;
}

//距離によってLODモデルの添え字を取得
uint32_t LODController::SelectLOD(float distance,uint32_t currentLOD,uint32_t lodCount) const {
	//LODが存在しないなら
	if(lodCount == 0) {
		return 0;
	}

	//currentLODが範囲外なら戻す
	if(currentLOD >= lodCount) {
		currentLOD = 0;
	}

	//LODモデル番号
	uint32_t result = currentLOD;

	//距離が登録されていないなら
	if(lodDistances_.empty()) {
		return 0;
	}

	//今の位置から見て
	//前に戻す
	if(currentLOD > 0) {
		if(distance < lodDistances_[currentLOD - 1] - hysteresis_) {
			result = currentLOD - 1;
		}
	}

	//先に進める
	if(currentLOD < lodCount - 1) {
		//距離が足りなくなった場合
		if(lodDistances_.size() <= currentLOD) {
			return result;
		} else if(distance >= lodDistances_[currentLOD] + hysteresis_) {
			result = currentLOD + 1;
		}
	}

	return result;
}

//LODの切り替え距離の設定
void LODController::SetLODDistances(const std::vector<float>& lodDistances) {
	lodDistances_ = lodDistances;
}