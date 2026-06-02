#include "Cube.h"
using namespace PrimitiveData;
using namespace Primitive;

//コンストラクタ
Cube::Cube() {}

//デストラクタ
Cube::~Cube() {}

//初期化
void Cube::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = 8;
	indexCount_ = 24;
	BaseShape::Initialize(directXBase, camera);

	//サイズを設定
	obb_.size = Vector3::MakeAllOne();
}

//更新
void Cube::Update() {
	//トランスフォームに送信
	transform_.quaternion = obb_.quaternion;
	transform_.translate = obb_.center;

	//基底クラスの更新
	BaseShape::Update();
}

//OBBのセッター
void Cube::SetOBB(const OBB& obb) {
	obb_ = obb;
}

//OBBのゲッター
OBB Cube::GetOBB() {
	return obb_;
}

//AABBのゲッター
AABB Cube::GetAABB() {
	//AABB
	AABB aabb = {
		{obb_.center - obb_.size},
		{obb_.center + obb_.size},
	};
	return aabb;
}

//頂点データの設定
void Cube::SettingVertexData() {
	//AABB
	AABB aabb = {
		{-obb_.size / 2.0f},
		{obb_.size / 2.0f},
	};
	////前面
	// 左上
	vertexData_[0].position = { aabb.min.x,aabb.min.y,aabb.min.z,1.0f };
	// 右上
	vertexData_[1].position = { aabb.max.x,aabb.min.y,aabb.min.z,1.0f };
	// 右下
	vertexData_[2].position = { aabb.max.x,aabb.max.y,aabb.min.z,1.0f };
	// 左下
	vertexData_[3].position = { aabb.min.x,aabb.max.y,aabb.min.z,1.0f };

	//背面
	// 左上
	vertexData_[4].position = { aabb.min.x,aabb.min.y,aabb.max.z,1.0f };
	// 右上
	vertexData_[5].position = { aabb.max.x,aabb.min.y,aabb.max.z,1.0f };
	// 右下
	vertexData_[6].position = { aabb.max.x,aabb.max.y,aabb.max.z,1.0f };
	// 左下
	vertexData_[7].position = { aabb.min.x,aabb.max.y,aabb.max.z,1.0f };

	//UVとNormalの初期化
	for (int32_t i = 0; i < vertexCount_; i++) {
		vertexData_[i].texcoord = { 0.0f,0.0f };
		vertexData_[i].normal = { 0.0f,0.0f,1.0f };
	}
}

//インデックスの設定
void Cube::SettingIndexData() {
	int32_t indices[] = {
		//前面
		0,1,
		1,2,
		2,3,
		3,0,

		//背面
		4,5,
		5,6,
		6,7,
		7,4,

		//接続
		0,4,
		1,5,
		2,6,
		3,7
	};

	//作成したインデックスデータを代入前面
	for (int32_t i = 0; i < indexCount_; i++) {
		indexData_[i] = indices[i];
	}
}