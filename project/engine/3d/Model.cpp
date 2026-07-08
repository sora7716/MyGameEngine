#define NOMINMAX
#include "Model.h"
#include "DirectXBase.h"
#include "ModelCommon.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Logger.h"
#include "MathUtility.h"
#include "HashUtility.h"
#include <format>
#include <map>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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

//プリミティブのメッシュ作成関数をまとめたテーブル
MeshData(Model::* Model::PrimitiveMeshFactoryTable[])() = {
	&MakeCubeData,
	&MakeSphereData,
};

//コンストラクタ
Model::Model() {
}

//デストラクタ
Model::~Model() {
}

//モデルの生成(ファイルを読み込んでの)
std::unique_ptr<Model>Model::CreateFromModel(ModelCommon* modelCommon, const std::string& storedFilePath, const std::string& filename) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateFromModel(storedFilePath, filename);
	return instance;
}

//モデルの生成(キューブ)
std::unique_ptr<Model> Model::CreatePrimitiveModel(ModelCommon* modelCommon, const PrimitiveMeshCreateDesc& meshCreateDesc) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->InitializePrimitiveModel(meshCreateDesc);
	return instance;
}

//モデルの生成(モデルデータ)
std::unique_ptr<Model> Model::CreateModelFromModelData(ModelCommon* modelCommon, const ModelData& modelData) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateModelFromModelData(modelData);
	return instance;
}

//初期化
void Model::Initialize(ModelCommon* modelCommon) {
	//ModelCommonのポインタを引数からメンバ変数を記録する
	modelCommon_ = modelCommon;
	//DirectXの基盤部分を受け取る
	directXBase_ = modelCommon_->GetDirectXBase();
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
	if (!modelData_.material.empty()) {
		for (uint32_t i = 0; i < modelData_.mesheDatas.size(); i++) {
			if (modelData_.mesheDatas[i].materialIndex >= modelData_.material.size()) {
				modelData_.mesheDatas[i].materialIndex = 0;
			}
		}
	} else {
		//マテリアルが存在しなかった場合
		MaterialData material;
#ifdef _DEBUG
		material.textureFilePath = "engine/resources/textures/magenta1x1.png";
#else
		material.textureFilePath = "engine/resources/textures/white1x1.png";
#endif // _DEBUG
		modelData_.material.push_back(material);
	}
	//メッシュを構築
	BuildMesh();
	//マテリアルリソースとポインタのサイズ設定
	materialResources_.resize(modelData_.material.size());
	materialPtrs_.resize(modelData_.material.size());
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

		MaterialData& materialData = modelData_.material[materialIndex];

		//テクスチャをセット
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, modelCommon_->GetTextureManager()->GetSRVHandleGPU(materialData.textureFilePath));
		mesh->Draw(objectCount);
	}
}

//uv変換
void Model::UVTransform(uint32_t index, Transform2d uvTransform) {
	materialPtrs_[index]->uvMatrix = MatrixUtility::MakeUVAffineMatrix(uvTransform);
}

// 色を変更
void Model::SetColor(uint32_t index, const Vector4& color) {
	materialPtrs_[index]->color = color;
}

//テクスチャの変更
void Model::SetTexture(uint32_t materialIndex, const std::string& imageFileName) {
	modelData_.material[materialIndex].textureFilePath = "engine/resources/textures/" + imageFileName;
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.material[materialIndex].textureFilePath);
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

//.mtlファイルの読み取り	
MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1.中で必要となる変数の宣言
	MaterialData materialData;//構築するMaterialData
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
ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& storedFilePath, const std::string& filename) {
	//構築するModelData
	ModelData modelData;

	//assimpでobjファイルを読み込む
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + storedFilePath + "/" + filename;
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
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		//マテリアルデータ
		MaterialData materialData;

		if (material->GetTextureCount(aiTextureType_BASE_COLOR) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &textureFilePath);

			materialData.textureFilePath = directoryPath + "/" + storedFilePath + "/" + textureFilePath.C_Str();
		} else if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			materialData.textureFilePath = directoryPath + "/" + storedFilePath + "/" + textureFilePath.C_Str();
		}

		//モデルデータのマテリアルにマテリアルデータを移動
		modelData.material.push_back(std::move(materialData));
	}

	return modelData;
}

//ライティングの設定
void Model::SetIsLighting(uint32_t index, bool isLighting) {
	materialPtrs_[index]->enableLighting = isLighting;
}

//輝度の設定
void Model::SetShininess(uint32_t index, float shininess) {
	materialPtrs_[index]->shininess = shininess;
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

//プリミティブメッシュタイプからメッシュ名を取得する
std::string Model::GetNameFromPrimitiveMeshType(PrimitiveMeshType meshType) {
	std::vector<std::string>meshNameList{
		"cube",
		"sphere"
	};
	return meshNameList[static_cast<uint32_t>(meshType)];
}

//マテリアルリソースの生成
void Model::CreateMaterialResource() {
	for (uint32_t i = 0; i < modelData_.material.size(); i++) {
		//マテリアル用のリソースを作る
		materialResources_[i] = directXBase_->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレスを取得
		materialResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialPtrs_[i]));
		//色を書き込む
		materialPtrs_[i]->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		materialPtrs_[i]->enableLighting = true;
		materialPtrs_[i]->uvMatrix = Matrix4x4::Identity4x4();
		materialPtrs_[i]->shininess = 10.0f;
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

//キューブの作成
MeshData Model::MakeCubeData() {
	//面のデータ
	struct FaceData {
		Vector3 normal;
		Vector4 position[4];
	};

	//どこの面
	enum FaceType :uint32_t {
		kFront,
		kBack,
		kRight,
		kLeft,
		kTop,
		kBottom,
		kFaceCount
	};

	//頂点の場所
	enum FaceRect :uint32_t {
		kLeftUp,
		kRightUp,
		kLeftBottom,
		kRightBottom,
		kFaceRectCount
	};

	//メッシュデータの頂点とインデックスのサイズ決定
	MeshData mesh = {};
	const uint32_t kVertexCount = 24;
	const uint32_t kIndexCount = 36;
	mesh.vertices.resize(24);
	mesh.indices.resize(36);

	//UV座標
	Vector2 uv[kFaceRectCount] = {};
	uv[kLeftUp] = { 0.0f,0.0f };//左上
	uv[kRightUp] = { 1.0f,0.0f };//右上
	uv[kLeftBottom] = { 0.0f,1.0f };//左下
	uv[kRightBottom] = { 1.0f,1.0f };//右下

	//法線と位置
	FaceData face[kFaceCount]{};

	//正面
	face[kFront].normal = { 0.0f, 0.0f, 1.0f };
	face[kFront].position[kLeftUp] = { -1.0f, 1.0f, 1.0f, 1.0f };
	face[kFront].position[kRightUp] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face[kFront].position[kLeftBottom] = { -1.0f, -1.0f, 1.0f, 1.0f };
	face[kFront].position[kRightBottom] = { 1.0f, -1.0f, 1.0f, 1.0f };

	//背面
	face[kBack].normal = { 0.0f, 0.0f, 1.0f };
	face[kBack].position[kLeftUp] = { -1.0f, 1.0f, -1.0f, 1.0f };
	face[kBack].position[kRightUp] = { 1.0f, 1.0f, -1.0f, 1.0f };
	face[kBack].position[kLeftBottom] = { -1.0f, -1.0f, -1.0f, 1.0f };
	face[kBack].position[kRightBottom] = { 1.0f, -1.0f, -1.0f, 1.0f };

	//右面
	face[kRight].normal = { 1.0f, 0.0f, 0.0f };
	face[kRight].position[kLeftUp] = { 1.0f, -1.0f, 1.0f, 1.0f };
	face[kRight].position[kRightUp] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face[kRight].position[kLeftBottom] = { 1.0f, -1.0f, -1.0f, 1.0f };
	face[kRight].position[kRightBottom] = { 1.0f, 1.0f, -1.0f, 1.0f };

	//左面
	face[kLeft].normal = { -1.0f, 0.0f, 0.0f };
	face[kLeft].position[kLeftUp] = { -1.0f, 1.0f, 1.0f, 1.0f };
	face[kLeft].position[kRightUp] = { -1.0f, 1.0f, -1.0f, 1.0f };
	face[kLeft].position[kLeftBottom] = { -1.0f, -1.0f, 1.0f, 1.0f };
	face[kLeft].position[kRightBottom] = { -1.0f, -1.0f, -1.0f, 1.0f };

	//上面
	face[kTop].normal = { 0.0f, 1.0f, 0.0f };
	face[kTop].position[kLeftUp] = { -1.0f, 1.0f, 1.0f, 1.0f };
	face[kTop].position[kRightUp] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face[kTop].position[kLeftBottom] = { -1.0f, 1.0f, -1.0f, 1.0f };
	face[kTop].position[kRightBottom] = { 1.0f, 1.0f, -1.0f, 1.0f };

	//下面
	face[kBottom].normal = { 0.0f, -1.0f, 0.0f };
	face[kBottom].position[kLeftUp] = { -1.0f, -1.0f, 1.0f, 1.0f };
	face[kBottom].position[kRightUp] = { 1.0f, -1.0f, 1.0f, 1.0f };
	face[kBottom].position[kLeftBottom] = { -1.0f, -1.0f, -1.0f, 1.0f };
	face[kBottom].position[kRightBottom] = { 1.0f, -1.0f, -1.0f, 1.0f };

	//頂点データの入力
	for (uint32_t faceIndex = 0; faceIndex < kFaceCount; faceIndex++) {
		for (uint32_t faceRectIndex = 0; faceRectIndex < kFaceRectCount; faceRectIndex++) {
			uint32_t index = faceIndex * 4 + faceRectIndex;
			mesh.vertices[index] = {
				.position = face[faceIndex].position[faceRectIndex],
				.texcoord = uv[faceRectIndex],
				.normal = face[faceIndex].normal,
			};
		}
	}

	// Z+  正面
	mesh.indices[0] = 0;
	mesh.indices[1] = 2;
	mesh.indices[2] = 1;
	mesh.indices[3] = 2;
	mesh.indices[4] = 3;
	mesh.indices[5] = 1;

	// Z-  背面
	mesh.indices[6] = 4;
	mesh.indices[7] = 5;
	mesh.indices[8] = 6;
	mesh.indices[9] = 6;
	mesh.indices[10] = 5;
	mesh.indices[11] = 7;

	// X+  右面
	mesh.indices[12] = 8;
	mesh.indices[13] = 10;
	mesh.indices[14] = 9;
	mesh.indices[15] = 10;
	mesh.indices[16] = 11;
	mesh.indices[17] = 9;

	// X-  左面
	mesh.indices[18] = 12;
	mesh.indices[19] = 13;
	mesh.indices[20] = 14;
	mesh.indices[21] = 14;
	mesh.indices[22] = 13;
	mesh.indices[23] = 15;

	// Y+  上面
	mesh.indices[24] = 16;
	mesh.indices[25] = 17;
	mesh.indices[26] = 18;
	mesh.indices[27] = 18;
	mesh.indices[28] = 17;
	mesh.indices[29] = 19;

	// Y-  下面
	mesh.indices[30] = 20;
	mesh.indices[31] = 22;
	mesh.indices[32] = 21;
	mesh.indices[33] = 22;
	mesh.indices[34] = 23;
	mesh.indices[35] = 21;
	return mesh;
}

//球の作成
MeshData Model::MakeSphereData() {
	//メッシュ
	MeshData meshData = {};
	const uint32_t kSubdivision = 16;
	meshData.vertices.resize(kSubdivision * kSubdivision * 6);

	//経度分割1つ分の角度φd
	float pi = MathUtility::kPi;
	const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision);
	//緯度分割1つぶんの角度θd
	const float kLatEvery = pi / static_cast<float>(kSubdivision);
	//緯度方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++) {
		//θ
		float lat = -pi / 2.0f + kLatEvery * static_cast<float>(latIndex);
		//緯度方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			//φ
			float lon = lonIndex * kLonEvery;
			//頂点データを入力する
			//基準点a
			meshData.vertices[start].position.x = std::cos(lat) * std::cos(lon);
			meshData.vertices[start].position.y = std::sin(lat);
			meshData.vertices[start].position.z = std::cos(lat) * std::sin(lon);
			meshData.vertices[start].position.w = 1.0f;
			meshData.vertices[start].texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision);
			meshData.vertices[start].texcoord.y = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision);
			meshData.vertices[start].normal.x = meshData.vertices[start].position.x;
			meshData.vertices[start].normal.y = meshData.vertices[start].position.y;
			meshData.vertices[start].normal.z = meshData.vertices[start].position.z;

			//b
			meshData.vertices[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			meshData.vertices[start + 1].position.y = std::sin(lat + kLatEvery);
			meshData.vertices[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			meshData.vertices[start + 1].position.w = 1.0f;
			meshData.vertices[start + 1].texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(kSubdivision);
			meshData.vertices[start + 1].texcoord.y = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision);
			meshData.vertices[start + 1].normal.x = meshData.vertices[start + 1].position.x;
			meshData.vertices[start + 1].normal.y = meshData.vertices[start + 1].position.y;
			meshData.vertices[start + 1].normal.z = meshData.vertices[start + 1].position.z;

			//c
			meshData.vertices[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			meshData.vertices[start + 2].position.y = std::sin(lat);
			meshData.vertices[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			meshData.vertices[start + 2].position.w = 1.0f;
			meshData.vertices[start + 2].texcoord.x = static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision);
			meshData.vertices[start + 2].texcoord.y = 1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision);
			meshData.vertices[start + 2].normal.x = meshData.vertices[start + 2].position.x;
			meshData.vertices[start + 2].normal.y = meshData.vertices[start + 2].position.y;
			meshData.vertices[start + 2].normal.z = meshData.vertices[start + 2].position.z;

			//d
			meshData.vertices[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			meshData.vertices[start + 3].position.y = std::sin(lat + kLatEvery);
			meshData.vertices[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			meshData.vertices[start + 3].position.w = 1.0f;
			meshData.vertices[start + 3].texcoord.x = static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision);
			meshData.vertices[start + 3].texcoord.y = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision);
			meshData.vertices[start + 3].normal.x = meshData.vertices[start + 3].position.x;
			meshData.vertices[start + 3].normal.y = meshData.vertices[start + 3].position.y;
			meshData.vertices[start + 3].normal.z = meshData.vertices[start + 3].position.z;
		}
	}

	//インデックス
	meshData.indices.resize(meshData.vertices.size());
	for (uint32_t i = 0; i < meshData.vertices.size() / 6; i++) {
		uint32_t start = i * 6;
		meshData.indices[start] = start;
		meshData.indices[start + 1] = start + 1;
		meshData.indices[start + 2] = start + 2;
		meshData.indices[start + 3] = start + 1;
		meshData.indices[start + 4] = start + 3;
		meshData.indices[start + 5] = start + 2;
	}

	return meshData;
}

//プリミティブモデルの初期化
void Model::InitializePrimitiveModel(const PrimitiveMeshCreateDesc& meshCreateDesc) {
	//モデルの読み込み
	modelData_.mesheDatas = { (this->*PrimitiveMeshFactoryTable[static_cast<uint32_t>(meshCreateDesc.meshType)])() };
	//メッシュの再構築
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
	//テクスチャの適応
	SetTexture(modelData_.mesheDatas[0].materialIndex, "white1x1.png");
	//ノードの初期化
	Node& node = modelData_.rootNode;
	node.name = GetNameFromPrimitiveMeshType(meshCreateDesc.meshType);
	node.localMatrix = Matrix4x4::Identity4x4();
}

//モデルの生成
void Model::CreateFromModel(const std::string& storedFilePath, const std::string& filename) {
	//モデルの読み込み
	modelData_ = LoadModelFile("engine/resources/models", storedFilePath, filename);
	//メッシュの再構築
	RebuildMeshes(modelData_.mesheDatas);
	//各種リソースの生成
	CreateResources();
}

//モデルの生成(モデルデータ)
void Model::CreateModelFromModelData(const ModelData& modelData) {
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
	for (MaterialData& materialData : modelData_.material) {
		modelCommon_->GetTextureManager()->LoadTexture(materialData.textureFilePath);
	}
}