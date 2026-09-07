#include "ModelLoader.h"
#include "MathUtility.h"
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
static Node ReadNode(aiNode* node){
	Node result;
	//nodeのlocalMatrixを取得
	aiMatrix4x4 aiLocalMatrix = node->mTransformation;
	//行列を転置
	aiLocalMatrix.Transpose();

	//aiMatrix4x4からMatrix4x4へ変換
	for (int32_t i = 0; i < 4; i++){
		for (int32_t j = 0; j < 4; j++){
			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
		}
	}

	//Zだけを反転させる
	Matrix4x4 flipZ = Matrix4x4::Identity4x4();
	flipZ.m[2][2] = -1.0f;

	//頂点と同じ座標系へ変換
	result.localMatrix = flipZ * result.localMatrix * flipZ;

	//Nodeの名前を取得
	result.name = node->mName.C_Str();
	//子の数だけ
	result.children.resize(node->mNumChildren);

	//このNodeが持つメッシュの数に合わせる
	result.meshIndices.resize(node->mNumMeshes);
	//各メッシュの番号を保存
	for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++){
		result.meshIndices[meshIndex] = node->mMeshes[meshIndex];
	}

	//再帰的に呼んで階層構造を作っていく
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++){
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}

//Nodeを巡回して、対応するメッシュへ変換を反映する
static void ApplyNodeTransform(Node& node, const Matrix4x4& parentMatrix, std::vector<MeshData>& meshDatas){
	//親までの変換を含んだ、このNodeの行列
	Matrix4x4 nodeMatrix = node.localMatrix * parentMatrix;

	for (uint32_t meshIndex : node.meshIndices){
		//nodeに入っているメッシュのインデックスに入っている各メッシュにアクセス
		assert(meshIndex < static_cast<uint32_t>(meshDatas.size()));
		MeshData & mesh = meshDatas[meshIndex];

		//メッシュの各頂点にnodeMatrixを適応
		for (VertexData& vertex : mesh.vertices){
			vertex.position = vertex.position * nodeMatrix;

			//法線の位置を正しくする
			Matrix4x4 normalMatrix = nodeMatrix.InverseTranspose();
			vertex.normal = mathUtility::TransformNormal(vertex.normal, normalMatrix).Normalize();
		}
	}

	//子Nodeには、現在のnodeMatrixを親行列として渡す
	for (Node& child : node.children){
		ApplyNodeTransform(child, nodeMatrix, meshDatas);
	}
}

//.mtlファイルの読み取り	
MaterialTexturePaths modelLoader::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename){
	//1.中で必要となる変数の宣言
	MaterialTexturePaths materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開かなかったら止める
	//3.実際にファイルを読み込み、MaterialDataを構築していく
	while (std::getline(file, line)){
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		//identifierに応じた処理
		if (identifier == "map_Kd"){
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
ModelData modelLoader::LoadModelFile(const std::string& directoryPath, const std::string& fileName){
	//構築するModelData
	ModelData modelData;

	//assimpでobjファイルを読み込む
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + fileName;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがない場合は終了

	//meshを解析する
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++){
		aiMesh* assimpMesh = scene->mMeshes[meshIndex];
		assert(assimpMesh->HasNormals());//法線がないメッシュは未対応
		assert(assimpMesh->HasTextureCoords(0));//Texcoordがないメッシュは未対応

		//メッシュデータ
		MeshData meshData;
		meshData.materialIndex = assimpMesh->mMaterialIndex;

		//頂点を見る
		for (uint32_t vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++){
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
		for (uint32_t faceIndex = 0; faceIndex < assimpMesh->mNumFaces; faceIndex++){
			aiFace& face = assimpMesh->mFaces[faceIndex];
			//三角形以外は未対応
			assert(face.mNumIndices == 3);

			meshData.indices.push_back(face.mIndices[0]);
			meshData.indices.push_back(face.mIndices[1]);
			meshData.indices.push_back(face.mIndices[2]);
		}

		//モデルデータにメッシュデータを移動
		modelData.meshDatas.push_back(std::move(meshData));
	}

	//RootNodeの解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	//RootNodeを各メッシュに適応
	ApplyNodeTransform(modelData.rootNode, Matrix4x4::Identity4x4(), modelData.meshDatas);

	//Node変換は頂点で焼きこみ済み
	modelData.rootNode.localMatrix = Matrix4x4::Identity4x4();

	//materialを解析
	//ディレクトリパスを作成
	std::filesystem::path folderPath = static_cast<std::filesystem::path>(fileName).parent_path();
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++){
		aiMaterial* material = scene->mMaterials[materialIndex];

		//マテリアルデータ
		MaterialTexturePaths materialData;

		if (material->GetTextureCount(aiTextureType_BASE_COLOR) != 0){
			aiString textureFilePath;
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &textureFilePath);

			materialData.textureFilePath = directoryPath + "/" + folderPath.string() + "/" + textureFilePath.C_Str();
		} else if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0){
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			materialData.textureFilePath = directoryPath + "/" + folderPath.string() + "/" + textureFilePath.C_Str();
		}

		//モデルデータのマテリアルにマテリアルデータを移動
		modelData.materialTexturePaths.push_back(std::move(materialData));
	}

	return modelData;
}
