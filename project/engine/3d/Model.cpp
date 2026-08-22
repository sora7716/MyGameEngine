#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "Mesh.h"
#include "PrimitiveMeshFactory.h"
#include "ModelLoader.h"
#include "MaterialInstance.h"

//モデルの生成(ファイルを読み込んでの)
std::unique_ptr<Model>Model::CreateModel(DirectXBase* directXBase, const std::string& modelFileName){
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(directXBase);
	//モデルの生成
	instance->CreateModel(modelFileName);
	return instance;
}

//モデルの生成(メッシュデータ)
std::unique_ptr<Model> Model::CreateModel(DirectXBase* directXBase, const std::vector<MeshData>& meshDatas){
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(directXBase);
	//モデルの生成
	instance->CreateModel(meshDatas);
	return instance;
}

//モデルの生成(モデルデータ)
std::unique_ptr<Model> Model::CreateModel(DirectXBase* directXBase, const ModelData& modelData){
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(directXBase);
	//モデルの生成
	instance->CreateModel(modelData);
	return instance;
}

//コンストラクタ
Model::Model(){
}

//デストラクタ
Model::~Model(){
}

//初期化
void Model::Initialize(DirectXBase* directXBase){
	//DirectXの基盤部分を記録
	assert(directXBase);
	directXBase_ = directXBase;
}

//メッシュの再構成
void Model::RebuildMeshes(const std::vector<MeshData>& meshes){
	//メッシュデータのクリア
	if (!meshes_.empty()){
		meshes_.clear();
	}
	//受け取ったメッシュデータに書き換え
	modelData_.meshDatas = meshes;
	//マテリアルが存在するか
	if (!modelData_.materialTexturePaths.empty()){
		for (uint32_t i = 0; i < modelData_.meshDatas.size(); i++){
			if (modelData_.meshDatas[i].materialIndex >= modelData_.materialTexturePaths.size()){
				modelData_.meshDatas[i].materialIndex = 0;
			}
		}
	} else{
		//マテリアルが存在しなかった場合
		MaterialTexturePaths material;
		material.environmentMap = "engine/resources/textures/skybox_cube.dds";
#ifdef _DEBUG
		material.textureFilePath = "engine/resources/textures/magenta1x1.png";
#else
		material.textureFilePath = "engine/resources/textures/white1x1.png";
#endif // _DEBUG
		modelData_.materialTexturePaths.push_back(material);
	}
	//メッシュを構築
	BuildMesh();
}

//描画に必要なデータのセットアップ
void Model::SetupRenderData(){
	//描画に必要なデータ
	modelRenderData_.meshRenderDatas.resize(meshes_.size());
	for (uint32_t i = 0; i < meshes_.size(); i++){
		modelRenderData_.meshRenderDatas[i] = meshes_[i]->GetMeshRenderData();
	}
	modelRenderData_.modelData = modelData_;
	modelRenderData_.rimLightResource = nullptr;
}

//モデルデータのゲッター
const ModelData& Model::GetModelData() const{
	return modelData_;
}

//メッシュたちのゲッター
const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const{
	return meshes_;
}

//描画に必要なデータの取得
const ModelRenderData& Model::GetModelRenderData(){
	return modelRenderData_;
}

//デフォルトのマテリアルインスタンスの取得
std::shared_ptr<MaterialInstance> Model::GetDefaultMaterialInstance() const{
	return defaultMaterialInstance_;
}

//メッシュの構築
void Model::BuildMesh(){
	//メッシュの生成と初期化
	meshes_.reserve(modelData_.meshDatas.size());
	for (const MeshData& meshData : modelData_.meshDatas){
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}
}

//プリミティブモデルの初期化
void Model::CreateModel(const std::vector<MeshData>& meshDatas, const std::string& nodeName){
	//モデルの読み込み
	modelData_.meshDatas = { meshDatas };
	//メッシュの再構築
	RebuildMeshes(modelData_.meshDatas);
	//テクスチャの設定
	modelData_.materialTexturePaths[0].textureFilePath = "engine/resources/textures/white1x1.png";
	//環境マップの設定
	modelData_.materialTexturePaths[0].environmentMap = "engine/resources/textures/skybox_cube.dds";
	//ノードの初期化
	Node& node = modelData_.rootNode;
	node.name = nodeName;
	node.localMatrix = Matrix4x4::Identity4x4();
	//マテリアルインスタンスの生成と初期化
	defaultMaterialInstance_ = std::make_shared<MaterialInstance>();
	defaultMaterialInstance_->Initialize(modelData_.materialTexturePaths);
	//描画データをまとめる
	SetupRenderData();
}

//モデルの生成
void Model::CreateModel(const std::string& objectFileName){
	//モデルの読み込み
	modelData_ = modelLoader::LoadModelFile("engine/resources/models", objectFileName);
	//メッシュの再構築
	RebuildMeshes(modelData_.meshDatas);
	//環境マップの設定
	for (MaterialTexturePaths& environmentMap : modelData_.materialTexturePaths){
		environmentMap.environmentMap = "engine/resources/textures/skybox_cube.dds";
	}
	//マテリアルインスタンスの生成と初期化
	defaultMaterialInstance_ = std::make_shared<MaterialInstance>();
	defaultMaterialInstance_->Initialize(modelData_.materialTexturePaths);
	//描画データをまとめる
	SetupRenderData();
}

//モデルの生成(モデルデータ)
void Model::CreateModel(const ModelData& modelData){
	modelData_ = modelData;
	//メッシュの再構成
	RebuildMeshes(modelData_.meshDatas);
	//環境マップの設定
	for (MaterialTexturePaths& environmentMap : modelData_.materialTexturePaths){
		environmentMap.environmentMap = "engine/resources/textures/skybox_cube.dds";
	}
	//マテリアルインスタンスの生成と初期化
	defaultMaterialInstance_ = std::make_shared<MaterialInstance>();
	defaultMaterialInstance_->Initialize(modelData_.materialTexturePaths);
	//描画データをまとめる
	SetupRenderData();
}