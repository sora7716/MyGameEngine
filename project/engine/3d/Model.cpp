#include "Model.h"
#include "DirectXBase.h"
#include "ModelCommon.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Logger.h"
#include <format>
#include <map>
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

//コンストラクタ
Model::Model() {
}

//デストラクタ
Model::~Model() {
}

//モデルの生成(ファイルを読み込んでの)
std::unique_ptr<Model> Model::CreateFromModel(ModelCommon* modelCommon, const std::string& storedFilePath, const std::string& filename) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateFromModel(storedFilePath, filename);
	return instance;
}

//モデルの生成(キューブ)
std::unique_ptr<Model> Model::CreateCube(ModelCommon* modelCommon) {
	//インスタンスの生成
	std::unique_ptr<Model>instance = std::make_unique<Model>();
	//初期化
	instance->Initialize(modelCommon);
	//モデルの生成
	instance->CreateCube();
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
	meshes_.clear();
	//受け取ったメッシュデータに書き換え
	modelData_.meshes = meshes;
	//マテリアルが存在するか
	if (!modelData_.material.empty()) {
		for (uint32_t i = 0; i < modelData_.meshes.size(); i++) {
			if (modelData_.meshes[i].materialIndex >= modelData_.material.size()) {
				modelData_.meshes[i].materialIndex = 0;
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

//頂点を合成する
std::vector<MeshData> Model::VertexClustering(float rate, float min, float max) {
	//メッシュ
	std::vector<MeshData>baseMeshes = modelData_.meshes;

	//新しいメッシュ
	std::vector<MeshData>newMeshes;

	//割合が1.0fより大きかったら
	if (rate > 1.0f) {
		Logger::ConsolePrintf("[Model::VertexClustering] rate is 1.0f. Skip clustering and return original meshes.\n");
		return baseMeshes;
	}

	//割合が0.0fより小さかった場合
	if (rate < 0.0f) {
		Logger::ConsolePrintf("[Model::VertexClustering] rate is 0.0f or less. Invalid rate. Return original meshes.\n");
		return baseMeshes;
	}

	//割合をもとに取得したい頂点数を出す
	for (const MeshData& meshData : baseMeshes) {
		uint32_t goalVertexCount = uint32_t(float(meshData.vertices.size()) * rate);
		//グリッドサイズ
		float minGridSize = min;//最小値
		float maxGridSize = max;//最大値

		//目標の頂点数に一番違いメッシュ
		MeshData bestMesh = meshData;
		uint32_t bestMeshVertexCount = uint32_t(bestMesh.vertices.size());
		float gridSize = 0;
		//試行回数
		const uint32_t kTrialCount = 20;
		//二分探索
		for (uint32_t i = 0; i < kTrialCount; i++) {
			//gridSizeはminとmaxの中間
			gridSize = (minGridSize + maxGridSize) / 2.0f;

			//元メッシュをgridSizeでクラスタリング
			MeshData trialMesh = VertexClusteringByGridSize(meshData, gridSize);
			//クラスタリングしたメッシュの頂点数を取得
			uint32_t trialMeshVertexCount = uint32_t(trialMesh.vertices.size());

			//目標の頂点数より差分が小さいほうのメッシュを入れる
			if (std::fabs(float(goalVertexCount) - float(bestMeshVertexCount)) > std::fabs(float(goalVertexCount) - float(trialMeshVertexCount))) {
				//ベストメッシュの置き換え
				bestMesh = trialMesh;
				//頂点の数の記録
				bestMeshVertexCount = uint32_t(bestMesh.vertices.size());
			}

			//範囲を狭めていく
			if (trialMeshVertexCount > goalVertexCount) {
				minGridSize = gridSize;
			} else if (trialMeshVertexCount < goalVertexCount) {
				maxGridSize = gridSize;
			}
		}
		//新しいメッシュに追加する
		newMeshes.push_back(bestMesh);
	}

	return newMeshes;
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
	materialPtrs_[index]->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform);
}

// 色を変更
void Model::SetColor(uint32_t index, const Vector4& color) {
	materialPtrs_[index]->color = color;
}

//テクスチャの変更
void Model::SetTexture(uint32_t index, const std::string& filePath) {
	modelData_.material[index].textureFilePath = "engine/resources/textures/" + filePath;
	modelCommon_->GetTextureManager()->LoadTexture(modelData_.material[index].textureFilePath);
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
		modelData.meshes.push_back(std::move(meshData));
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

//マテリアルのセッター
void Model::SetMaterial(uint32_t index, const Material& materialData) {
	materialPtrs_[index]->color = materialData.color;
	materialPtrs_[index]->enableLighting = materialData.enableLighting;
	materialPtrs_[index]->shininess = materialData.shininess;
	materialPtrs_[index]->uvMatrix = materialData.uvMatrix;
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
	for (const MeshData& meshData : modelData_.meshes) {
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

//キューブの生成
void Model::CreateCube() {
	//マテリアルの初期化
	MaterialData material;
	material.srvIndex = 0;
	material.textureFilePath = "engine/resources/textures/white1x1.png";
	//マテリアルを設定
	modelData_.material.push_back(material);
	//メッシュの再構築
	RebuildMeshes({ MakeCubeData() });
	//各種リソースの生成
	CreateResourcees();
}

//モデルの生成
void Model::CreateFromModel(const std::string& storedFilePath, const std::string& filename) {
	//モデルの読み込み
	modelData_ = LoadModelFile("engine/resources/models", storedFilePath, filename);
	//メッシュの再構築
	RebuildMeshes(modelData_.meshes);
	//各種リソースの生成
	CreateResourcees();
}

//各種リソースの生成
void Model::CreateResourcees() {
	//マテリアルリソースの生成
	CreateMaterialResource();
	//リムライトリソースの生成
	CreateRimLightResource();
	//テクスチャの読み込み
	for (MaterialData& materialData : modelData_.material) {
		modelCommon_->GetTextureManager()->LoadTexture(materialData.textureFilePath);
	}
}

//近くの頂点をまとめる
MeshData Model::VertexClusteringByGridSize(const MeshData& meshData, float size) {
	//GridKeyの構造体
	struct GridKey {
		Vector3Int vertexKey;
		Vector2Int uvKey;
		Vector3Int normalKey;
	};

	//メッシュを取得
	MeshData mesh = meshData;

	//Gridサイズの設定
	float gridSize = size;
	//GridKeyの一覧表
	std::map<GridKey, uint32_t>gridToNewIndex;
	//前のインデックスから新しいインデックスを取得するための対応表
	std::vector<uint32_t>oldToNewIndex(mesh.vertices.size(), UINT32_MAX);
	//新しい頂点
	std::vector<VertexData>newVertices;
	//新しインデックス
	uint32_t newIndex = 0;
	//GridKeyの作成
	for (uint32_t oldIndex = 0; oldIndex < mesh.vertices.size(); oldIndex++) {
		//一つの頂点
		VertexData vertex = mesh.vertices[oldIndex];

		//GridKeyの作成
		GridKey gridKey = {};

		//頂点のキーを作成
		gridKey.vertexKey  = {
			static_cast<int32_t>(std::floor(vertex.position.x / gridSize)),
			static_cast<int32_t>(std::floor(vertex.position.y / gridSize)),
			static_cast<int32_t>(std::floor(vertex.position.z / gridSize)),
		};

		//UVのキーを作成
		//法線のキーを作成

		//gridKeyが一覧表に登録されていたら
		if (gridToNewIndex.contains(gridKey)) {
			//登録済みのインデックスを追加
			oldToNewIndex[oldIndex] = gridToNewIndex[gridKey];
			continue;
		}

		//未登録なら
		//新しい頂点に追加
		newVertices.push_back(vertex);
		//グリッドの一覧表に新しいインデックスを追加
		gridToNewIndex[gridKey] = newIndex;
		//対応表に新しいインデックスを追加
		oldToNewIndex[oldIndex] = newIndex;
		//新しいインデックスの加算
		newIndex++;
	}

	//インデックスの張替え
	std::vector<uint32_t>newIndices;
	for (uint32_t i = 0; i < mesh.indices.size(); i += 3) {
		uint32_t a = oldToNewIndex[mesh.indices[i]];
		uint32_t b = oldToNewIndex[mesh.indices[i + 1]];
		uint32_t c = oldToNewIndex[mesh.indices[i + 2]];

		//三角形が作れない場合は省く
		if (a == b) {
			continue;
		} else if (a == c) {
			continue;
		} else if (b == c) {
			continue;
		}

		//新しいインデックスを追加
		newIndices.push_back(a);
		newIndices.push_back(b);
		newIndices.push_back(c);
	}

	//頂点に代入
	mesh.vertices = newVertices;
	//インデックスに代入
	mesh.indices = newIndices;

	return mesh;
}
