#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "Mesh.h"
#include "PrimitiveMeshFactory.h"
#include "ModelLoader.h"

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

//モデルの生成(キューブ)
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

	//描画に必要なデータのセットアップ
	SetupRenderData();
}

//描画に必要なデータのセットアップ
void Model::SetupRenderData(){
	//描画に必要なデータ
	modelRenderData_.materialResources = materialResources_;
	modelRenderData_.meshRenderDatas.resize(meshes_.size());
	for (uint32_t i = 0; i < meshes_.size(); i++){
		modelRenderData_.meshRenderDatas[i] = meshes_[i]->GetMeshRenderData();
	}
	modelRenderData_.modelData = modelData_;
	modelRenderData_.rimLightResource = rimLightResource_;
}

//uv変換
void Model::UVTransform(uint32_t index, Transform2d uvTransform){
	materialPtrs_[index]->uvMatrix = matrixUtility::MakeUVAffineMatrix(uvTransform);
}

// 色を変更
void Model::SetColor(uint32_t index, const Vector4& color){
	materialPtrs_[index]->color = color;
}

//テクスチャの設定
void Model::SetTexture(uint32_t materialIndex, const std::string& imageFileName){
	modelData_.materialTexturePaths[materialIndex].textureFilePath = "engine/resources/textures/" + imageFileName;
}

//環境マップの設定
void Model::SetEnvironmentMap(uint32_t materialIndex, const std::string& environmentMapFileName){
	modelData_.materialTexturePaths[materialIndex].environmentMap = "engine/resources/textures/" + environmentMapFileName;
}

//色を取得
const Vector4& Model::GetColor(uint32_t index) const{
	// TODO: return ステートメントをここに挿入します
	return materialPtrs_[index]->color;
}

//モデルデータのゲッター
const ModelData& Model::GetModelData() const{
	// TODO: return ステートメントをここに挿入します
	return modelData_;
}

//ライティングの設定
void Model::SetIsLighting(uint32_t materialIndex, bool isLighting){
	materialPtrs_[materialIndex]->enableLighting = isLighting;
}

//輝度の設定
void Model::SetShininess(uint32_t materialIndex, float shininess){
	materialPtrs_[materialIndex]->shininess = shininess;
}

//環境マップの映り込み度を調整
void Model::SetEnvironmentCoefficient(uint32_t materialIndex, float environmentCoefficient){
	materialPtrs_[materialIndex]->environmentCoefficient = environmentCoefficient;
}

//リムライトのセッター
void Model::SetRimLight(const RimLight& rimLight){
	rimLightPtr_->color = rimLight.color;
	rimLightPtr_->outLinePower = rimLight.outLinePower;
	rimLightPtr_->power = rimLight.power;
	rimLightPtr_->softness = rimLight.softness;
	rimLightPtr_->enableRimLighting = rimLight.enableRimLighting;
}

//メッシュたちのゲッター
const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const{
	return meshes_;
}

//描画に必要なデータの取得
const ModelRenderData& Model::GetModelRenderData(){
	// TODO: return ステートメントをここに挿入します
	return modelRenderData_;
}

//マテリアルリソースの生成
void Model::CreateMaterialResource(){
	//マテリアルリソースとポインタのサイズ設定
	materialResources_.resize(modelData_.materialTexturePaths.size());
	materialPtrs_.resize(modelData_.materialTexturePaths.size());
	for (uint32_t i = 0; i < modelData_.materialTexturePaths.size(); i++){
		//マテリアル用のリソースを作る
		materialResources_[i] = directXBase_->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレスを取得
		materialResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialPtrs_[i]));
		//色を書き込む
		materialPtrs_[i]->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		materialPtrs_[i]->enableLighting = true;
		materialPtrs_[i]->uvMatrix = Matrix4x4::Identity4x4();
		materialPtrs_[i]->shininess = 10.0f;
		materialPtrs_[i]->environmentCoefficient = 0.0f;
	}
}

//リムライトのリソースを生成
void Model::CreateRimLightResource(){
	//マテリアル用のリソースを作る
	rimLightResource_ = directXBase_->CreateBufferResource(sizeof(RimLight));
	//書き込むためのアドレスを取得
	rimLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&rimLightPtr_));
	//色を書き込む
	rimLightPtr_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	rimLightPtr_->outLinePower = 0.1f;
	rimLightPtr_->power = 0.1f;
	rimLightPtr_->softness = 5.0f;
	rimLightPtr_->enableRimLighting = false;
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
	//各種リソースの生成
	CreateResources();
	//テクスチャの適応
	SetTexture(modelData_.meshDatas[0].materialIndex, "white1x1.png");
	//環境マッピング
	SetEnvironmentMap(modelData_.meshDatas[0].materialIndex, "skybox_cube.dds");
	//ノードの初期化
	Node& node = modelData_.rootNode;
	node.name = nodeName;
	node.localMatrix = Matrix4x4::Identity4x4();
}

//モデルの生成
void Model::CreateModel(const std::string& objectFileName){
	//モデルの読み込み
	modelData_ = modelLoader::LoadModelFile("engine/resources/models", objectFileName);
	//メッシュの再構築
	RebuildMeshes(modelData_.meshDatas);
	//各種リソースの生成
	CreateResources();
	for (MeshData& meshData : modelData_.meshDatas){
		SetEnvironmentMap(meshData.materialIndex, "skybox_cube.dds");
	}
}

//モデルの生成(モデルデータ)
void Model::CreateModel(const ModelData& modelData){
	modelData_ = modelData;
	//メッシュの再構成
	RebuildMeshes(modelData_.meshDatas);
	//各種リソースの生成
	CreateResources();
	for (MeshData& meshData : modelData_.meshDatas){
		SetEnvironmentMap(meshData.materialIndex, "skybox_cube.dds");
	}
}

//各種リソースの生成
void Model::CreateResources(){
	//マテリアルリソースの生成
	CreateMaterialResource();
	//リムライトリソースの生成
	CreateRimLightResource();
}