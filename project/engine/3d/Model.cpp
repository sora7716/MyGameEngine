#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "ModelCommon.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "PrimitiveMeshFactory.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
/// <summary>
/// assimpのノードを読み取る
/// </summary>
/// <param name="node">assimpのノード</param>
/// <returns>ノード</returns>
static Node ReadNode(aiNode* node) {
	Node result;
	//nodeのlocalMatrixを取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	//行列を転置
	aiLocalMatrix.Transpose();

	//aiMatrix4x4からMatrix4x4へ変換
	for (int32_t i = 0; i < 4; i++) {
		for (int32_t j = 0; j < 4; j++) {
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}
	//Nodeの名前を取得
	result.name = node->mName.C_Str();
	//子の数だけ
	result.children.resize(node->mNumChildren);

	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		//再帰的に呼んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

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

//.mtlファイルの読み取り	
MaterialTexturePaths Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1.中で必要となる変数の宣言
	MaterialTexturePaths materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開かなかったら止める
	//3.実際にファイルを読み込み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//4.MaterialDataを返す
	return materialData;
}

//モデルファイルの読み込み
ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& fileName) {
	//構築するModelData
	ModelData modelData;

	//assimpでobjファイルを読み込む
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + fileName;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがない場合は終了

	//meshを解析する
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* assimpMesh = scene->mMeshes[meshIndex];
		assert(assimpMesh->HasNormals());//法線がないメッシュは未対応
		assert(assimpMesh->HasTextureCoords(0));//Texcoordがないメッシュは未対応

		//メッシュデータ
		MeshData meshData;
		meshData.materialIndex = assimpMesh->mMaterialIndex;

		//頂点を見る
		for (uint32_t vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++) {
			aiVector3D& position = assimpMesh->mVertices[vertexIndex];
			aiVector3D& normal = assimpMesh->mNormals[vertexIndex];
			aiVector3D& texcoord = assimpMesh->mTextureCoords[0][vertexIndex];

			VertexData vertex = {};
			vertex.position = { position.x,position.y,position.z,1.0f };
			vertex.normal = { normal.x,normal.y,normal.z };
			vertex.texcoord = { texcoord.x, texcoord.y };

			//右手座標系から見だりて座標系に直してる
			vertex.position.z *= -1.0f;
			vertex.normal.z *= -1.0f;

			meshData.vertices.push_back(vertex);
		}

		//インデックスを見る
		for (uint32_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++) {
			aiFace& face = assimpMesh->mFaces[faceIndex];
			//三角形以外は未対応
			assert(face.mNumIndices == 3);

			meshData.indices.push_back(face.mIndices[0]);
			meshData.indices.push_back(face.mIndices[1]);
			meshData.indices.push_back(face.mIndices[2]);
		}

		//モデルデータにメッシュデータを移動
		modelData.mesheDatas.push_back(std::move(meshData));
	}

	//RootNodeの解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	//materialを解析
	//ディレクトリパスを作成
	std::filesystem::path folderPath = static_cast<std::filesystem::path>(fileName).parent_path();
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		//マテリアルデータ
		MaterialTexturePaths materialData;

		if (material->GetTextureCount(aiTextureType_BASE_COLOR) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &textureFilePath);

			materialData.textureFilePath = directoryPath + "/" + folderPath.string() + "/" + textureFilePath.C_Str();
		} else if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			materialData.textureFilePath = directoryPath + "/" + folderPath.string() + "/" + textureFilePath.C_Str();
		}

		//モデルデータのマテリアルにマテリアルデータを移動
		modelData.materialTexturePaths.push_back(std::move(materialData));
	}

	return modelData;
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
	modelData_.mesheDatas = meshes;
	//マテリアルが存在するか
	if (!modelData_.materialTexturePaths.empty()) {
		for (uint32_t i = 0; i < modelData_.mesheDatas.size(); i++) {
			if (modelData_.mesheDatas[i].materialIndex >= modelData_.materialTexturePaths.size()) {
				modelData_.mesheDatas[i].materialIndex = 0;
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
	meshes_.reserve(modelData_.mesheDatas.size());
	for (const MeshData& meshData : modelData_.mesheDatas) {
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}
}

//プリミティブモデルの初期化
void Model::CreateModel(const std::vector<MeshData>& meshDatas, const std::string& nodeName) {
	//モデルの読み込み
	modelData_.mesheDatas = { primitiveMeshFactory::CreatePlane() };
	//メッシュの再構築
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
	//テクスチャの適応
	SetTexture(modelData_.mesheDatas[0].materialIndex, "white1x1.png");
	//環境マッピング
	SetEnvironmentMap(modelData_.mesheDatas[0].materialIndex, "rostock_laage_airport_4k.dds");
	//ノードの初期化
	Node& node = modelData_.rootNode;
	node.name = nodeName;
	node.localMatrix = Matrix4x4::Identity4x4();
}

//モデルの生成
void Model::CreateModel(const std::string& objectFileName) {
	//モデルの読み込み
	modelData_ = LoadModelFile("engine/resources/models", objectFileName);
	//メッシュの再構築
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
}

//モデルの生成(モデルデータ)
void Model::CreateModel(const ModelData& modelData) {
	modelData_ = modelData;
	//メッシュの再構成
	RebuildMeshes(modelData_.mesheDatas);
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