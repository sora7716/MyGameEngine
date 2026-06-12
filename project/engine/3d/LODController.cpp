#include "LODController.h"
#include "LODBuilder.h"
#include <cassert>

//初期化
void LODController::Initialize(LODBuilder* lodBuilder, const std::vector<float>& lodDistances) {
	//LODビルダーの記録
	assert(lodBuilder);
	lodBuilder_ = lodBuilder;
	//LOD距離の記録
	lodDistances_ = lodDistances;
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
	//ヒステリシス幅
	const float hysteresis = 5.0f;

	//今の位置から見て
	//前に戻す
	if (currentLOD > 0) {
		if (distance < lodDistances_[currentLOD] - hysteresis) {
			result = currentLOD - 1;
		}
	}

	//先に進める
	if (distance >= lodDistances_[currentLOD + 1] + hysteresis) {
		result = currentLOD + 1;
	}

	//選ばれたLODがなければ
	if (!lodBuilder_->GetLODModel(result)) {
		return 0;
	}

	return result;
}
