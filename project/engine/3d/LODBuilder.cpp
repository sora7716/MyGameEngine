#include "LODBuilder.h"
#include "Logger.h"
#include "Model.h"

//コンストラクタ
LODBuilder::LODBuilder() {
}

//デストラクタ
LODBuilder::~LODBuilder() {
}

//LODモデルの生成
void LODBuilder::CreateLODModel(Model* model, const std::vector<float>& keepRates) {
	//頂点合成する割合が存在しなかったら
	if (keepRates.empty()) {
		return;
	}

	//サイズを決定
	lodModels_.resize(keepRates.size());
	//モデルの作成
	for (uint32_t i = 0; i < keepRates.size(); i++) {
		lodModels_[i] = Model::CreateModelFromModelData(model->GetModelCommon(), model->GetModelData());
		lodModels_[i]->RebuildMeshes(lodModels_[i]->VertexClustering(keepRates[i]));
		//lodModels_[i]->RebuildMeshes(lodModels_[i]->EdgeCollapse(keepRates[i]));
	}
}

//LODモデルの取得
Model* LODBuilder::GetLODModel(uint32_t lodIndex) {
	return lodModels_[lodIndex].get();
}

//カラーの設定
void LODBuilder::SetColor(uint32_t materialIndex, const Vector4& color) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetColor(materialIndex, color);
		}
	}
}

//テクスチャの設定
void LODBuilder::SetTexture(uint32_t materialIndex, const std::string& filePath) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetTexture(materialIndex, filePath);
		}
	}
}

//ライティングフラグの設定
void LODBuilder::SetIsLighting(uint32_t materialIndex, bool isLighting) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetIsLighting(materialIndex, isLighting);
		}
	}
}

//輝度の設定
void LODBuilder::SetShininess(uint32_t materialIndex, float shininess) {
	for (std::unique_ptr<Model>& lodModel : lodModels_) {
		if (lodModel) {
			lodModel->SetShininess(materialIndex, shininess);
		}
	}
}

//モデルのサイズ
uint32_t LODBuilder::LODModelSize()const {
	return static_cast<uint32_t>(lodModels_.size());
}
