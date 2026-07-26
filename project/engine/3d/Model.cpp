#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "ModelCommon.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "PrimitiveMeshFactory.h"
#include "ModelLoader.h"
//モデルの生成(ファイルを読み込んでの)
std::unique_ptr<Model>Model::CreateModel(ModelCommon* modelCommon, const std::string& modelFileName) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateModel(modelFileName);
	return instance;
}

//モデルの生成(キューブ)
std::unique_ptr<Model> Model::CreateModel(ModelCommon* modelCommon, const std::vector<MeshData>& meshDatas) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateModel(meshDatas);
	return instance;
}

//モデルの生成(モデルデータ)
std::unique_ptr<Model> Model::CreateModel(ModelCommon* modelCommon, const ModelData& modelData) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateModel(modelData);
	return instance;
}

//コンストラクタ
Model::Model() {
}

//デストラクタ
Model::~Model() {
}

//初期化
void Model::Initialize(ModelCommon* modelCommon) {
	//ModelCommonのポインタを引数からメンバ変数を記録する
	modelCommon_ = modelCommon;
	//DirectXの基盤部分を受け取る
	directXBase_ = modelCommon_->GetDirectXBase();
}

//描画
void Model::Draw(uint32_t objectCount) {
	//リムライトのCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(7, rimLightResource_->GetGPUVirtualAddress());
	//メッシュの描画
	for (std::unique_ptr<Mesh>& mesh : meshes_) {
		uint32_t materialIndex = mesh->GetMaterialIndex();

		//マテリアルCBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[materialIndex]->GetGPUVirtualAddress());

		MaterialTexturePaths& materialTexturePath = modelData_.materialTexturePaths[materialIndex];

		//テクスチャをセット
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, modelCommon_->GetTextureManager()->GetSRVHandleGPU(materialTexturePath.textureFilePath));

		//環境マップのセット
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(8, modelCommon_->GetTextureManager()->GetSRVHandleGPU(materialTexturePath.environmentMap));
		mesh->Draw(objectCount);
	}
}

//メッシュの再構成
void Model::RebuildMeshes(const std::vector<MeshData>& meshes) {
	//メッシュデータのクリア
	if (!meshes_.empty()) {
		meshes_.clear();
	}
	//受け取ったメッシュデータに書き換え
	modelData_.meshDatas = meshes;
	//マテリアルが存在するか
	if (!modelData_.materialTexturePaths.empty()) {
		for (uint32_t i = 0; i < modelData_.meshDatas.size(); i++) {
			if (modelData_.meshDatas[i].materialIndex >= modelData_.materialTexturePaths.size()) {
				modelData_.meshDatas[i].materialIndex = 0;
			}
		}
	} else {
		//マテリアルが存在しなかった場合
		MaterialTexturePaths material;
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

//uv変換
void Model::UVTransform(uint32_t index, Transform2d uvTransform) {
	materialPtrs_[index]->uvMatrix = matrixUtility::MakeUVAffineMatrix(uvTransform);
}

// 色を変更
void Model::SetColor(uint32_t index, const Vector4& color) {
	materialPtrs_[index]->color = color;
}

//テクスチャの設定
void Model::SetTexture(uint32_t materialIndex, const std::string& imageFileName) {
	modelData_.materialTexturePaths[materialIndex].textureFilePath = "engine/resources/textures/" + imageFileName;
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.materialTexturePaths[materialIndex].textureFilePath);
}

//環境マップの設定
void Model::SetEnvironmentMap(uint32_t materialIndex, const std::string& environmentMapFileName) {
	modelData_.materialTexturePaths[materialIndex].environmentMap = "engine/resources/textures/" + environmentMapFileName;
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.materialTexturePaths[materialIndex].environmentMap);
}

//色を取得
const Vector4& Model::GetColor(uint32_t index) const {
	// TODO: return ステートメントをここに挿入します
	return materialPtrs_[index]->color;
}

//モデルデータのゲッター
const ModelData& Model::GetModelData() const {
	// TODO: return ステートメントをここに挿入します
	return modelData_;
}

//ライティングの設定
void Model::SetIsLighting(uint32_t materialIndex, bool isLighting) {
	materialPtrs_[materialIndex]->enableLighting = isLighting;
}

//輝度の設定
void Model::SetShininess(uint32_t materialIndex, float shininess) {
	materialPtrs_[materialIndex]->shininess = shininess;
}

//環境マップの映り込み度を調整
void Model::SetEnvironmentCoefficient(uint32_t materialIndex, float environmentCoefficient) {
	materialPtrs_[materialIndex]->environmentCoefficient = environmentCoefficient;
}

//リムライトのセッター
void Model::SetRimLight(const RimLight& rimLight) {
	rimLightPtr_->color = rimLight.color;
	rimLightPtr_->outLinePower = rimLight.outLinePower;
	rimLightPtr_->power = rimLight.power;
	rimLightPtr_->softness = rimLight.softness;
	rimLightPtr_->enableRimLighting = rimLight.enableRimLighting;
}

//メッシュたちのゲッター
const std::vector<std::unique_ptr<Mesh>>& Model::GetMeshes() const {
	return meshes_;
}

//モデルの共通部分の取得
ModelCommon* Model::GetModelCommon() {
	return modelCommon_;
}

//マテリアルリソースの生成
void Model::CreateMaterialResource() {
	//マテリアルリソースとポインタのサイズ設定
	materialResources_.resize(modelData_.materialTexturePaths.size());
	materialPtrs_.resize(modelData_.materialTexturePaths.size());
	for (uint32_t i = 0; i < modelData_.materialTexturePaths.size(); i++) {
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
void Model::CreateRimLightResource() {
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
void Model::BuildMesh() {
	//メッシュの生成と初期化
	meshes_.reserve(modelData_.meshDatas.size());
	for (const MeshData& meshData : modelData_.meshDatas) {
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}
}

//プリミティブモデルの初期化
void Model::CreateModel(const std::vector<MeshData>& meshDatas, const std::string& nodeName) {
	//モデルの読み込み
	modelData_.meshDatas = { meshDatas };
	//メッシュの再構築
	RebuildMeshes(modelData_.meshDatas);
	//各種リソースの生成
	CreateResources();
	//テクスチャの適応
	SetTexture(modelData_.meshDatas[0].materialIndex, "white1x1.png");
	//環境マッピング
	SetEnvironmentMap(modelData_.meshDatas[0].materialIndex, "rostock_laage_airport_4k.dds");
	//ノードの初期化
	Node& node = modelData_.rootNode;
	node.name = nodeName;
	node.localMatrix = Matrix4x4::Identity4x4();
}

//モデルの生成
void Model::CreateModel(const std::string& objectFileName) {
	//モデルの読み込み
	modelData_ = modelLoader::LoadModelFile("engine/resources/models", objectFileName);
	//メッシュの再構築
	RebuildMeshes(modelData_.meshDatas);
	//各種リソースの生成
	CreateResources();
}

//モデルの生成(モデルデータ)
void Model::CreateModel(const ModelData& modelData) {
	modelData_ = modelData;
	//メッシュの再構成
	RebuildMeshes(modelData_.meshDatas);
	//各種リソースの生成
	CreateResources();
}

//各種リソースの生成
void Model::CreateResources() {
	//マテリアルリソースの生成
	CreateMaterialResource();
	//リムライトリソースの生成
	CreateRimLightResource();
	//テクスチャの読み込み
	for (MaterialTexturePaths& materialData : modelData_.materialTexturePaths) {
		modelCommon_->GetTextureManager()->LoadTexture(materialData.textureFilePath);
	}
}