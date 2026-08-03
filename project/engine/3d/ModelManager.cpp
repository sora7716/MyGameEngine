#include "ModelManager.h"
#include "ModelCommon.h"
#include "Model.h"
#include "PrimitiveMeshFactory.h"
#include <cassert>

//デストラクタ
ModelManager::~ModelManager(){}

//初期化
void ModelManager::Initialize(DirectXBase* directXBase, TextureManager* textureManager){
	//DirectXの基盤部分の記録
	assert(directXBase);
	directXBase_ = directXBase;
	//テクスチャの管理の記録
	assert(textureManager);
	textureManager_ = textureManager;

}

//プリミティブなモデルの生成
void ModelManager::CreatePrimitiveModel(){
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model>model = Model::CreateModel(directXBase_, textureManager_, { primitiveMeshFactory::CreateSphere() });

	//モデルデータを取得
	ModelData modelData = model->GetModelData();

	//モデルデータをmapコンテナに格納する
	modelDatas_.insert(std::make_pair("cube", modelData));
}

// objモデルの読み込み
void ModelManager::LoadModel(const std::string& name, const std::string& modelFileName){
	//読み込み済みならモデルを検索
	if (modelDatas_.contains(name)){
		//読み込み済みなら早期return
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model>model = Model::CreateModel(directXBase_, textureManager_, modelFileName);

	//モデルデータを取得
	ModelData modelData = model->GetModelData();

	//モデルデータをmapコンテナに格納する
	modelDatas_.insert(std::make_pair(name, modelData));
}

//モデルの検索
std::unique_ptr<Model> ModelManager::FindModel(const std::string& name){
	//読み込み済みモデルを検索
	if (modelDatas_.contains(name)){
		//読み込み済みモデルを戻り値としてreturn
		return std::move(Model::CreateModel(directXBase_, textureManager_, modelDatas_.at(name)));
	}
	//ファイル名一致なし
	return nullptr;
}

//コンストラクタ
ModelManager::ModelManager(ConstructorKey){}
