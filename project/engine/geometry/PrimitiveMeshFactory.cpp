#include "PrimitiveMeshFactory.h"
#include "MathUtility.h"
#include <algorithm>

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
	uv[kLeftUp] = {0.0f,0.0f};//左上
	uv[kRightUp] = {1.0f,0.0f};//右上
	uv[kLeftBottom] = {0.0f,1.0f};//左下
	uv[kRightBottom] = {1.0f,1.0f};//右下

	//法線と位置
	FaceData face[kFaceCount]{};

	//正面
	face[kFront].normal = {0.0f,0.0f,1.0f};
	face[kFront].position[kLeftUp] = {-1.0f,1.0f,1.0f,1.0f};
	face[kFront].position[kRightUp] = {1.0f,1.0f,1.0f,1.0f};
	face[kFront].position[kLeftBottom] = {-1.0f,-1.0f,1.0f,1.0f};
	face[kFront].position[kRightBottom] = {1.0f,-1.0f,1.0f,1.0f};

	//背面
	face[kBack].normal = {0.0f,0.0f,-1.0f};
	face[kBack].position[kLeftUp] = {-1.0f,1.0f,-1.0f,1.0f};
	face[kBack].position[kRightUp] = {1.0f,1.0f,-1.0f,1.0f};
	face[kBack].position[kLeftBottom] = {-1.0f,-1.0f,-1.0f,1.0f};
	face[kBack].position[kRightBottom] = {1.0f,-1.0f,-1.0f,1.0f};

	//右面
	face[kRight].normal = {1.0f,0.0f,0.0f};
	face[kRight].position[kLeftUp] = {1.0f,-1.0f,1.0f,1.0f};
	face[kRight].position[kRightUp] = {1.0f,1.0f,1.0f,1.0f};
	face[kRight].position[kLeftBottom] = {1.0f,-1.0f,-1.0f,1.0f};
	face[kRight].position[kRightBottom] = {1.0f,1.0f,-1.0f,1.0f};

	//左面
	face[kLeft].normal = {-1.0f,0.0f,0.0f};
	face[kLeft].position[kLeftUp] = {-1.0f,1.0f,1.0f,1.0f};
	face[kLeft].position[kRightUp] = {-1.0f,1.0f,-1.0f,1.0f};
	face[kLeft].position[kLeftBottom] = {-1.0f,-1.0f,1.0f,1.0f};
	face[kLeft].position[kRightBottom] = {-1.0f,-1.0f,-1.0f,1.0f};

	//上面
	face[kTop].normal = {0.0f,1.0f,0.0f};
	face[kTop].position[kLeftUp] = {-1.0f,1.0f,1.0f,1.0f};
	face[kTop].position[kRightUp] = {1.0f,1.0f,1.0f,1.0f};
	face[kTop].position[kLeftBottom] = {-1.0f,1.0f,-1.0f,1.0f};
	face[kTop].position[kRightBottom] = {1.0f,1.0f,-1.0f,1.0f};

	//下面
	face[kBottom].normal = {0.0f,-1.0f,0.0f};
	face[kBottom].position[kLeftUp] = {-1.0f,-1.0f,1.0f,1.0f};
	face[kBottom].position[kRightUp] = {1.0f,-1.0f,1.0f,1.0f};
	face[kBottom].position[kLeftBottom] = {-1.0f,-1.0f,-1.0f,1.0f};
	face[kBottom].position[kRightBottom] = {1.0f,-1.0f,-1.0f,1.0f};

	//頂点データの入力
	for(uint32_t faceIndex = 0; faceIndex < kFaceCount; faceIndex++) {
		for(uint32_t faceRectIndex = 0; faceRectIndex < kFaceRectCount; faceRectIndex++) {
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
	meshData.vertices.resize(desc.radialSubdivision * desc.radialSubdivision * 6);

	//経度分割1つ分の角度φd
	float pi = mathUtility::kPi;
	const float kLonEvery = pi * 2.0f / static_cast<float>(desc.radialSubdivision);
	//緯度分割1つぶんの角度θd
	const float kLatEvery = pi / static_cast<float>(desc.radialSubdivision);
	//緯度方向に分割
	for(uint32_t latIndex = 0; latIndex < desc.radialSubdivision; latIndex++) {
		//θ
		float lat = -pi / 2.0f + kLatEvery * static_cast<float>(latIndex);
		//緯度方向に分割しながら線を描く
		for(uint32_t lonIndex = 0; lonIndex < desc.radialSubdivision; lonIndex++) {
			uint32_t start = (latIndex * desc.radialSubdivision + lonIndex) * 6;
			//φ
			float lon = lonIndex * kLonEvery;
			//頂点データを入力する
			//基準点a
			meshData.vertices[start].position.x = std::cos(lat) * std::cos(lon);
			meshData.vertices[start].position.y = std::sin(lat);
			meshData.vertices[start].position.z = std::cos(lat) * std::sin(lon);
			meshData.vertices[start].position.w = 1.0f;
			meshData.vertices[start].texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start].texcoord.y = 1.0f - static_cast<float>(latIndex) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start].normal.x = meshData.vertices[start].position.x;
			meshData.vertices[start].normal.y = meshData.vertices[start].position.y;
			meshData.vertices[start].normal.z = meshData.vertices[start].position.z;

			//b
			meshData.vertices[start + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
			meshData.vertices[start + 1].position.y = std::sin(lat + kLatEvery);
			meshData.vertices[start + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
			meshData.vertices[start + 1].position.w = 1.0f;
			meshData.vertices[start + 1].texcoord.x = static_cast<float>(lonIndex) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start + 1].texcoord.y = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start + 1].normal.x = meshData.vertices[start + 1].position.x;
			meshData.vertices[start + 1].normal.y = meshData.vertices[start + 1].position.y;
			meshData.vertices[start + 1].normal.z = meshData.vertices[start + 1].position.z;

			//c
			meshData.vertices[start + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
			meshData.vertices[start + 2].position.y = std::sin(lat);
			meshData.vertices[start + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
			meshData.vertices[start + 2].position.w = 1.0f;
			meshData.vertices[start + 2].texcoord.x = static_cast<float>(lonIndex + 1) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start + 2].texcoord.y = 1.0f - static_cast<float>(latIndex) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start + 2].normal.x = meshData.vertices[start + 2].position.x;
			meshData.vertices[start + 2].normal.y = meshData.vertices[start + 2].position.y;
			meshData.vertices[start + 2].normal.z = meshData.vertices[start + 2].position.z;

			//d
			meshData.vertices[start + 3].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			meshData.vertices[start + 3].position.y = std::sin(lat + kLatEvery);
			meshData.vertices[start + 3].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			meshData.vertices[start + 3].position.w = 1.0f;
			meshData.vertices[start + 3].texcoord.x = static_cast<float>(lonIndex + 1) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start + 3].texcoord.y = 1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(desc.radialSubdivision);
			meshData.vertices[start + 3].normal.x = meshData.vertices[start + 3].position.x;
			meshData.vertices[start + 3].normal.y = meshData.vertices[start + 3].position.y;
			meshData.vertices[start + 3].normal.z = meshData.vertices[start + 3].position.z;
		}
	}

	//半径を適応
	for(VertexData& vertexData : meshData.vertices) {
		vertexData.position.SetVector3(vertexData.position.GetVector3() * desc.radius);
	}

	//インデックス
	meshData.indices.resize(meshData.vertices.size());
	for(uint32_t i = 0; i < meshData.vertices.size() / 6; i++) {
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
		.normal = {0.0f,0.0f,-1.0f}
	};
	//右上
	meshData.vertices[1] = {
		.position = {1.0f,1.0f,0.0f,1.0f},
		.texcoord = {1.0f,0.0f},
		.normal = {0.0f,0.0f,-1.0f}
	};
	//右下
	meshData.vertices[2] = {
		.position = {1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {1.0f,1.0f},
		.normal = {0.0f,0.0f,-1.0f}
	};
	//左下
	meshData.vertices[3] = {
		.position = {-1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {0.0f,1.0f},
		.normal = {0.0f,0.0f,-1.0f}
	};

	//サイズを適応
	for(VertexData& vertexData : meshData.vertices) {
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

//カプセルメッシュの生成
MeshData primitiveMeshFactory::CreateCapsule(const Desc& desc) {
	MeshData mesh = {};

	const uint32_t radial = desc.radialSubdivision;
	const uint32_t vertical = desc.verticalSubdivision;
	const float radius = desc.radius;
	const float halfCylinderHeight = 1.0f;
	const float pi = mathUtility::kPi;

	if(radial < 3 || vertical == 0 || radius <= 0.0f) {
		return mesh;
	}

	const uint32_t verticesPerRing = radial + 1;
	const uint32_t ringsPerHemisphere = vertical + 1;
	const uint32_t ringCount = ringsPerHemisphere * 2;

	mesh.vertices.reserve(ringCount * verticesPerRing);

	// 下端 → 下の赤道 → 上の赤道 → 上端
	for(uint32_t ring = 0; ring < ringCount; ++ring) {
		const bool isUpper = ring >= ringsPerHemisphere;
		const uint32_t localRing = isUpper ? ring - ringsPerHemisphere : ring;
		const float t = static_cast<float>(localRing) / static_cast<float>(vertical);

		// 下半球は -90°〜0°、上半球は 0°〜90°。
		const float latitude = (isUpper ? t : t - 1.0f) * (pi * 0.5f);
		const float centerY = isUpper ? halfCylinderHeight : -halfCylinderHeight;
		const bool isPole = ring == 0 || ring == ringCount - 1;
		const float horizontal = isPole ? 0.0f : std::cos(latitude);
		const float normalY = std::sin(latitude);

		for(uint32_t column = 0; column <= radial; ++column) {
			const float u = static_cast<float>(column) / static_cast<float>(radial);
			// 継ぎ目では位置を揃え、UVだけ0と1で分ける。
			const float longitude = column == radial ? 0.0f : u * (2.0f * pi);

			const float normalX = horizontal * std::cos(longitude);
			const float normalZ = horizontal * std::sin(longitude);

			VertexData vertex = {};
			vertex.position = {
				radius * normalX,
				centerY + radius * normalY,
				radius * normalZ,
				1.0f
			};
			vertex.normal = {normalX,normalY,normalZ};
			vertex.texcoord = {
				u,
				1.0f - static_cast<float>(ring) / static_cast<float>(ringCount - 1)
			};

			mesh.vertices.push_back(vertex);
		}
	}

	// 隣り合う輪の4頂点を、三角形2枚でつなぐ。
	for(uint32_t ring = 0; ring + 1 < ringCount; ++ring) {
		for(uint32_t column = 0; column < radial; ++column) {
			const uint32_t a = ring * verticesPerRing + column;
			const uint32_t b = a + 1;
			const uint32_t c = a + verticesPerRing;
			const uint32_t d = c + 1;

			// 下端では a と b が同じ位置。
			if(ring != 0) {
				mesh.indices.push_back(a);
				mesh.indices.push_back(c);
				mesh.indices.push_back(b);
			}

			// 上端では c と d が同じ位置。
			if(ring + 1 != ringCount - 1) {
				mesh.indices.push_back(b);
				mesh.indices.push_back(c);
				mesh.indices.push_back(d);
			}
		}
	}

	return mesh;
}

//扇形の生成
MeshData primitiveMeshFactory::CreateSector(const Desc& desc) {
	//メッシュ
	MeshData mesh = {};

	//半分の角度を取得
	const float halfDegree = desc.sweepAngleDegrees * 0.5f;

	//原点の追加
	VertexData origin = {
		.position = {0.0f,0.0f,0.0f,1.0f},
		.texcoord = {0.5f + origin.position.x * 0.5f,1.0f - origin.position.z},
		.normal = Vector3::GetUp(),
	};
	mesh.vertices.push_back(origin);

	//弧の頂点を作成(頂点の作成)
	for(uint32_t i = 0; i <= desc.radialSubdivision; i++) {
		VertexData vertex = {};

		//割合を求める
		const float rate = static_cast<float>(i) / static_cast<float>(desc.radialSubdivision);
		//角度を求める
		const float degree = std::lerp(-halfDegree,halfDegree,rate);
		//度数法を弧度法へ変換
		const float radian = degree * mathUtility::kRad;

		//位置
		vertex.position.x = desc.radius * std::sin(radian);
		vertex.position.y = 0.0f;
		vertex.position.z = desc.radius * std::cos(radian);
		vertex.position.w = 1.0f;

		//テクスチャ座標
		vertex.texcoord.x = 0.5f + vertex.position.x * 0.5f;
		vertex.texcoord.y = 1.0f - vertex.position.z;

		//法線
		vertex.normal = Vector3::GetUp();

		//メッシュの頂点データに追加
		mesh.vertices.push_back(vertex);
	}

	//インデックスの作成
	for(uint32_t i = 0; i < desc.radialSubdivision; i++) {
		mesh.indices.push_back(0);//扇の先端
		mesh.indices.push_back(i + 1);//弧の頂点
		mesh.indices.push_back(i + 2);//孫隣の頂点
	}

	return mesh;
}
