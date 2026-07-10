#include "PrimitiveMeshFactory.h"
#include "MathUtility.h"

//キューブ
MeshData primitiveMeshFactory::CreateCube(const Desc& desc) {
	//半分のサイズ
	Vector3 halfSize = desc.size / 2.0f;

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
	mesh.vertices.resize(kVertexCount);
	mesh.indices.resize(kIndexCount);

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
	face[kBack].normal = { 0.0f, 0.0f, -1.0f };
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
			//サイズを適応
			mesh.vertices[index].position.SetVector3(mesh.vertices[index].position.GetVector3() * halfSize);
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

//球メッシュの作成
MeshData primitiveMeshFactory::CreateSphere(const Desc& desc) {
	//メッシュ
	MeshData meshData = {};
	meshData.vertices.resize(desc.subdivision * desc.subdivision * 6);

	//経度分割1つ分の角度φd
	float pi = mathUtility::kPi;
	const float kLonEvery = pi * 2.0f / static_cast<float>(desc.subdivision);
	//緯度分割1つぶんの角度θd
	const float kLatEvery = pi / static_cast<float>(desc.subdivision);
	//緯度方向に分割
	for (uint32_t latIndex = 0; latIndex < desc.subdivision; latIndex++) {
		//θ
		float lat = -pi / 2.0f + kLatEvery * static_cast<float>(latIndex);
		//緯度方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < desc.subdivision; lonIndex++) {
			uint32_t start = (latIndex * desc.subdivision + lonIndex) * 6;
			//φ
			float lon = lonIndex * kLonEvery;
			//頂点データを入力する
			//基準点a
			meshData.vertices[start].position.x = std::cos(lat) * std::cos(lon);
			meshData.vertices[start].position.y = std::sin(lat);
			meshData.vertices[start].position.z = std::cos(lat) * std::sin(lon);
			meshData.vertices[start].position.w = 1.0f;
			meshData.vertices[start].texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(desc.subdivision);
			meshData.vertices[start].texcoord.y = 1.0f - static_cast<float>(latIndex) / static_cast<float>(desc.subdivision);
			meshData.vertices[start].normal.x = meshData.vertices[start].position.x;
			meshData.vertices[start].normal.y = meshData.vertices[start].position.y;
			meshData.vertices[start].normal.z = meshData.vertices[start].position.z;

			//b
			meshData.vertices[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			meshData.vertices[start + 1].position.y = std::sin(lat + kLatEvery);
			meshData.vertices[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			meshData.vertices[start + 1].position.w = 1.0f;
			meshData.vertices[start + 1].texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(desc.subdivision);
			meshData.vertices[start + 1].texcoord.y = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(desc.subdivision);
			meshData.vertices[start + 1].normal.x = meshData.vertices[start + 1].position.x;
			meshData.vertices[start + 1].normal.y = meshData.vertices[start + 1].position.y;
			meshData.vertices[start + 1].normal.z = meshData.vertices[start + 1].position.z;

			//c
			meshData.vertices[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			meshData.vertices[start + 2].position.y = std::sin(lat);
			meshData.vertices[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			meshData.vertices[start + 2].position.w = 1.0f;
			meshData.vertices[start + 2].texcoord.x = static_cast<float>(lonIndex + 1) / static_cast<float>(desc.subdivision);
			meshData.vertices[start + 2].texcoord.y = 1.0f - static_cast<float>(latIndex) / static_cast<float>(desc.subdivision);
			meshData.vertices[start + 2].normal.x = meshData.vertices[start + 2].position.x;
			meshData.vertices[start + 2].normal.y = meshData.vertices[start + 2].position.y;
			meshData.vertices[start + 2].normal.z = meshData.vertices[start + 2].position.z;

			//d
			meshData.vertices[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			meshData.vertices[start + 3].position.y = std::sin(lat + kLatEvery);
			meshData.vertices[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			meshData.vertices[start + 3].position.w = 1.0f;
			meshData.vertices[start + 3].texcoord.x = static_cast<float>(lonIndex + 1) / static_cast<float>(desc.subdivision);
			meshData.vertices[start + 3].texcoord.y = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(desc.subdivision);
			meshData.vertices[start + 3].normal.x = meshData.vertices[start + 3].position.x;
			meshData.vertices[start + 3].normal.y = meshData.vertices[start + 3].position.y;
			meshData.vertices[start + 3].normal.z = meshData.vertices[start + 3].position.z;
		}
	}

	//半径を適応
	for (VertexData& vertexData : meshData.vertices) {
		vertexData.position.SetVector3(vertexData.position.GetVector3() * desc.radius);
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

//平面メッシュの作成
MeshData primitiveMeshFactory::CreatePlane(const Desc& desc) {
	MeshData meshData = {};

	Vector3 halfSize = desc.size / 2.0f;

	//配列の要素数を決定
	meshData.vertices.resize(4);
	meshData.indices.resize(6);

	//頂点
	//左上
	meshData.vertices[0] = {
		.position = {-1.0f,1.0f,0.0f,1.0f},
		.texcoord = {0.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
	};
	//右上
	meshData.vertices[1] = {
		.position = {1.0f,1.0f,0.0f,1.0f},
		.texcoord = {1.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
	};
	//右下
	meshData.vertices[2] = {
		.position = {1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {1.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
	};
	//左下
	meshData.vertices[3] = {
		.position = {-1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {0.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
	};

	//サイズを適応
	for (VertexData& vertexData : meshData.vertices) {
		vertexData.position.SetVector3(vertexData.position.GetVector3() * halfSize);
	}

	//インデックス
	meshData.indices[0] = 0;
	meshData.indices[1] = 1;
	meshData.indices[2] = 2;
	meshData.indices[3] = 0;
	meshData.indices[4] = 2;
	meshData.indices[5] = 3;

	return meshData;
}
