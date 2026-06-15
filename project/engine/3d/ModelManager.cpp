#include "ModelManager.h"
#include "engine/3d/ModelCommon.h"
#include "engine/3d/Model.h"
#include <cassert>

//デストラクタ
ModelManager::~ModelManager() {}

//初期化
void ModelManager::Initialize(ModelCommon* modelCommon) {
	assert(modelCommon);
	modelCommon_ = modelCommon;
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model>model = Model::CreateCube(modelCommon_);

	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair("cube", std::move(model)));
}

// objモデルの読み込み
void ModelManager::LoadModel(const std::string& name, const std::string& storedFileName, const std::string& filePath) {
	//読み込み済みならモデルを検索
	if (models_.contains(name)) {
		//読み込み済みなら早期return
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model>model = Model::CreateFromModel(modelCommon_, storedFileName, filePath);

	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair(name, std::move(model)));
}

//モデルの検索
Model* ModelManager::FindModel(const std::string& name) {
	//読み込み済みモデルを検索
	if (models_.contains(name)) {
		//読み込み済みモデルを戻り値としてreturn
		return models_.at(name).get();
	}
	//ファイル名一致なし
	return nullptr;
}

//モデルの共通部分のゲッター
ModelCommon* ModelManager::GetModelCommon() {
	return modelCommon_;
}

//コンストラクタ
ModelManager::ModelManager(ConstructorKey) {}
