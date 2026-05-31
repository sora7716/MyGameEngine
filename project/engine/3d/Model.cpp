#include "Model.h"
#include "DirectXBase.h"
#include "ModelCommon.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Mesh.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
/// <summary>
/// assimpのノードを読み取る
/// </summary>
/// <param name="node">assimpのノード</param>
/// <returns>ノード</returns>
Node ReadNode(aiNode* node) {
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

//コンストラクタ
Model::Model() {
}

//デストラクタ
Model::~Model() {
}

//初期化
void Model::Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& storedFilePath, const std::string& filename) {
	//ModelCommonのポインタを引数からメンバ変数を記録する
	modelCommon_ = modelCommon;
	//DirectXの基盤部分を受け取る
	directXBase_ = modelCommon_->GetDirectXBase();
	//モデルの読み込み
	modelData_ = LoadModelFile(directoryPath, storedFilePath, filename);
	//メッシュの生成
	mesh_ = std::make_unique<Mesh>();
	mesh_->Initialize(directXBase_, modelData_.mesh);
	//マテリアルリソースの生成
	CreateMaterialResource();
	//リムライトリソースの生成
	CreateRimLightResource();
	//テクスチャの読み込み
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.material.textureFilePath);
}

//描画
void Model::Draw(uint32_t objectCount) {
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//リムライトのCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(7, rimLightResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, modelCommon_->GetTextureManager()->GetSRVHandleGPU(modelData_.material.textureFilePath));
	//描画
	mesh_->Draw(objectCount);
}

//uv変換
void Model::UVTransform(Transform2dData uvTransform) {
	materialPtr_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform);
}

// 色を変更
void Model::SetColor(const Vector4& color) {
	materialPtr_->color = color;
}

//テクスチャの変更
void Model::SetTexture(const std::string& filePath) {
	modelData_.material.textureFilePath = "engine/resources/textures/" + filePath;
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.material.textureFilePath);
}

//色を取得
const Vector4& Model::GetColor() const {
	// TODO: return ステートメントをここに挿入します
	return materialPtr_->color;
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
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないメッシュは未対応
		assert(mesh->HasTextureCoords(0));//Texcoordがないメッシュは未対応

		//頂点データのオフセット
		uint32_t vertexOffset = static_cast<uint32_t>(modelData.mesh.vertices.size());

		//頂点を見る
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			VertexData vertex = {};
			vertex.position = { position.x,position.y,position.z,1.0f };
			vertex.normal = { normal.x,normal.y,normal.z };
			vertex.texcoord = { texcoord.x,texcoord.y };

			//右手座標系から見だりて座標系に直してる
			vertex.position *= -1.0f;
			vertex.position.w = 1.0f;
			vertex.normal *= -1.0f;

			modelData.mesh.vertices.push_back(vertex);
		}

		//インデックスを見る
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];
			//三角形以外は未対応
			assert(face.mNumIndices == 3);

			modelData.mesh.indices.push_back(vertexOffset + face.mIndices[0]);
			modelData.mesh.indices.push_back(vertexOffset + face.mIndices[1]);
			modelData.mesh.indices.push_back(vertexOffset + face.mIndices[2]);
		}
	}

	//RootNodeの解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	//materialを解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directoryPath + "/" + storedFilePath + "/" + textureFilePath.C_Str();
		}
	}

	return modelData;
}

//マテリアルのセッター
void Model::SetMaterial(const Material& materialData) {
	materialPtr_->color = materialData.color;
	materialPtr_->enableLighting = materialData.enableLighting;
	materialPtr_->shininess = materialData.shininess;
	materialPtr_->uvMatrix = materialData.uvMatrix;
}


//リムライトのセッター
void Model::SetRimLight(const RimLight& rimLight) {
	rimLightPtr_->color = rimLight.color;
	rimLightPtr_->outLinePower = rimLight.outLinePower;
	rimLightPtr_->power = rimLight.power;
	rimLightPtr_->softness = rimLight.softness;
	rimLightPtr_->enableRimLighting = rimLight.enableRimLighting;
}

//マテリアルリソースの生成
void Model::CreateMaterialResource() {
	//マテリアル用のリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialPtr_));
	//色を書き込む
	materialPtr_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialPtr_->enableLighting = true;
	materialPtr_->uvMatrix = Matrix4x4::Identity4x4();
	materialPtr_->shininess = 10.0f;
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
