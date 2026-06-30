#include "LODController.h"
#include "LODBuilder.h"
#include <cassert>

//初期化
void LODController::Initialize(LODBuilder* lodBuilder) {
	//LODビルダーの記録
	assert(lodBuilder);
	lodBuilder_ = lodBuilder;
}

//距離によってLODモデルの添え字を取得
uint32_t LODController::SelectLOD(float distance, uint32_t currentLOD) const {
	//LODが存在しないなら
	if (lodBuilder_->LODModelSize() <= 0) {
		return 0;
	}

	//距離境界が足りない場合
	if (lodDistances_.size() < lodBuilder_->LODModelSize() - 1) {
		return 0;
	}

	//currentLODが範囲外なら戻す
	if (currentLOD >= lodBuilder_->LODModelSize()) {
		currentLOD = 0;
	}

	//LODモデル番号
	uint32_t result = currentLOD;

	//今の位置から見て
	//前に戻す
	if (currentLOD > 0) {
		if (distance < lodDistances_[currentLOD - 1] - hysteresis_) {
			result = currentLOD - 1;
		}
	}

	//先に進める
	if (currentLOD < 2) {
		if (distance >= lodDistances_[currentLOD] + hysteresis_) {
			result = currentLOD + 1;
		}
	}

	//選ばれたLODがなければ
	if (!lodBuilder_->GetLODModel(result)) {
		return 0;
	}

	return result;
}

//LODの切り替え距離の設定
void LODController::SetLODDistances(const std::vector<float>& lodDistances) {
	lodDistances_ = lodDistances;
}

//ヒステリシス幅の設定
void LODController::SetHysteresis(float hysteresis) {
	hysteresis_ = hysteresis;
}
