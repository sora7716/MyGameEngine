#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "Mesh.h"
#include "Logger.h"
#include "ModelLoader.h"
#include "MaterialInstance.h"
#include "LODBuilder.h"
#include <algorithm>
#include <functional>

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

	//テクスチャまたは環境マップのパスが空だった場合
	for (MaterialTexturePaths& texturePath : modelData_.materialTexturePaths){
		//テクスチャ
		if (texturePath.textureFilePath.empty()){
#ifdef _DEBUG
			texturePath.textureFilePath = "engine/resources/textures/magenta1x1.png";
#else
			texturePath.textureFilePath = "engine/resources/textures/white1x1.png";
#endif // _DEBUG
		}

		//環境マップ
		if (texturePath.environmentMap.empty()){
			texturePath.environmentMap = "engine/resources/textures/skybox_cube.dds";
		}
	}

	//メッシュを構築
	BuildMesh();

	//描画データを修正
	SetupRenderData();
}

//LODモデルの生成
void Model::CreateLODModels(const std::vector<float>& keepRates){
	//LODモデルの生成
	std::vector<float>lodRate;
	for (float rate : keepRates){
		if (rate >= 1.0f){
			continue;
		}
		lodRate.push_back(rate);
	}
	//ソート(降順)
	std::sort(lodRate.begin(), lodRate.end(), std::greater<float>());

	//LODがもうすでに作られているのなら
	if (isLODGenerated_){
		//比較用のRateともともとのRateのサイズを比べて
		if (lodRate.size() != generatedLODRates_.size()){
			Logger::OutputLog("LODの倍率が違う");
			assert(false);
			return;
		}

		//各要素に入っている値を比べて
		for (uint32_t i = 0; i < static_cast<uint32_t>(generatedLODRates_.size()); i++){
			if (lodRate[i] != generatedLODRates_[i]){
				Logger::OutputLog("LODの倍率が違う");
				assert(false);
				return;
			}
		}

		Logger::OutputLog("LOD生成済みなのでスキップした");
		return;
	}

	Logger::OutputLog("LODを新しく生成した");

	//LODビルダーの生成
	lodBuilder_ = std::make_unique<LODBuilder>();

	//LODの倍率を保存
	generatedLODRates_ = lodRate;

	lodBuilder_->CreateLODModel(directXBase_, this, lodRate);

	//LODを作成したらtrueにする
	isLODGenerated_ = true;
}

//LODモデルの取得
Model* Model::GetLODModel(uint32_t lodIndex){
	//LODビルダーがNullなら
	if (!lodBuilder_){
		return this;
	}

	//LODの検索キーが0なら
	if (lodIndex == 0){
		return this;
	}

	//LODの検索キーがLODのサイズより多いなら
	if (lodIndex >= GetLODCount()){
		return this;
	}

	return lodBuilder_->GetLODModel(lodIndex - 1);
}

//LODの数の取得
uint32_t Model::GetLODCount() const{
	//LODビルダーがNullなら
	if (!lodBuilder_){
		return 1;
	}

	return 1 + lodBuilder_->LODModelSize();
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

//モデルの作成(メッシュデータから)
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
	node.baseMatrix = Matrix4x4::Identity4x4();
	node.meshIndices.clear();
	node.meshIndices.reserve(modelData_.meshDatas.size());
	for (uint32_t meshIndex = 0; meshIndex < static_cast<uint32_t>(modelData_.meshDatas.size()); meshIndex++){
		node.meshIndices.push_back(meshIndex);
	}

	//マテリアルインスタンスの生成と初期化
	defaultMaterialInstance_ = std::make_shared<MaterialInstance>();
	defaultMaterialInstance_->Initialize(modelData_.materialTexturePaths);
	//描画データをまとめる
	SetupRenderData();
}

//モデルの生成(モデルのファイルから)
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