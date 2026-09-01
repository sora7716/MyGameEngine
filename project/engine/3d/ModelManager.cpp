#include "ModelManager.h"
#include "Model.h"
#include "PrimitiveMeshFactory.h"
#include "TextureManager.h"
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
	//モデル
	std::unique_ptr<Model>model = nullptr;
	//設定項目
	primitiveMeshFactory::Desc desc = {};

	//Cubeモデルの生成(半径1.0)
	model = Model::CreateModel(directXBase_, { primitiveMeshFactory::CreateCube() });
	//Cubeモデルの追加(半径1.0)
	models_.insert(std::make_pair("cube", std::move(model)));

	//Tileモデルの生成(半径0.5)
	desc.size = { 0.5f,0.5f,0.5f };
	model = Model::CreateModel(directXBase_, { primitiveMeshFactory::CreateCube(desc) });
	//Tileモデルの追加(半径0.5)
	models_.insert(std::make_pair("tile", std::move(model)));

	//Sphereモデルの生成(分割数16)
	model = Model::CreateModel(directXBase_, { primitiveMeshFactory::CreateSphere() });
	//Sphereモデル(分割数16)の追加
	models_.insert(std::make_pair("sphere_16", std::move(model)));

	//Sphereモデルの生成(分割数32)
	model = Model::CreateModel(directXBase_, { primitiveMeshFactory::CreateSphere({Vector3::MakeAllOne(),32,1.0f}) });
	//Sphereモデル(分割数32)の追加
	models_.insert(std::make_pair("sphere_32", std::move(model)));

	//Planeモデルの生成
	model = Model::CreateModel(directXBase_, { primitiveMeshFactory::CreatePlane() });
	//Planeモデルの追加
	models_.insert(std::make_pair("plane", std::move(model)));
}

//モデルの追加
void ModelManager::AddModel(const std::string& name, const std::string& modelFileName){
	//読み込み済みならモデルを検索
	if (models_.contains(name)){
		//読み込み済みなら早期return
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model>model = Model::CreateModel(directXBase_, modelFileName);

	//テクスチャの読み込み
	for (const MaterialTexturePaths& textureFilePaths : model->GetModelData().materialTexturePaths){
		textureManager_->AddTexture(textureFilePaths.textureFilePath);
	}

	//モデルをmapコンテナに格納する
	models_.insert(std::make_pair(name, std::move(model)));
}

//モデルの検索
Model* ModelManager::FindModel(const std::string& name){
	auto it = models_.find(name);

	//モデルのイテレーターが末尾と一緒だった場合
	if (it == models_.end()){
		return nullptr;
	}

	return it->second.get();
}

//コンストラクタ
ModelManager::ModelManager(ConstructorKey){}
