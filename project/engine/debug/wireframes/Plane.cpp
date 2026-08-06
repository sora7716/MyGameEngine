#include "Plane.h"
//コンストラクタ
DebugDraw::Plane::Plane() {}

//デストラクタ
DebugDraw::Plane::~Plane() {}

//初期化
void DebugDraw::Plane::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = 4;
	indexCount_ = 8;
	plane_ = {
		.normal = {0.0f,1.0f,0.0f},
		.distance = 0.0f
	};
	BaseShape::Initialize(directXBase, camera);
}

//更新
void DebugDraw::Plane::Update() {
	//基底クラスの更新
	BaseShape::Update();
}

//平面の設定
void DebugDraw::Plane::SetPlane(const primitiveData::Plane& plane) {
	plane_ = plane;
}

//平面の取得
const primitiveData::Plane& DebugDraw::Plane::GetPlane() const {
	// TODO: return ステートメントをここに挿入します
	return plane_;
}

//頂点データの設定
void DebugDraw::Plane::SettingVertexData() {
	Vector3 normal = plane_.normal.Normalize();

	//平面上の中心点
	Vector3 center = normal * plane_.distance;

	//平面上の横方向
	Vector3 tangent = Perpendicular(normal).Normalize();

	//平面上の縦方向
	Vector3 bitangent = normal.Cross(tangent).Normalize();

	float halfSize = 5.0f;

	Vector3 positions[4] = {
		center + tangent * halfSize + bitangent * halfSize,
		center - tangent * halfSize + bitangent * halfSize,
		center - tangent * halfSize - bitangent * halfSize,
		center + tangent * halfSize - bitangent * halfSize,
	};

	for (uint32_t i = 0; i < 4; i++) {
		vertexData_[i].position = {
			positions[i].x,
			positions[i].y,
			positions[i].z,
			1.0f
		};

		vertexData_[i].normal = normal;
		vertexData_[i].texcoord = { 0.0f,0.0f };
	}
}

//インデックスの設定
void DebugDraw::Plane::SettingIndexData() {
	int32_t indices[] = {
		//前面
		0,1,
		1,2,
		2,3,
		3,0,
	};

	//作成したインデックスデータを代入前面
	for (int32_t i = 0; i < indexCount_; i++) {
		indexData_[i] = indices[i];
	}

}

//垂直の処理
Vector3 DebugDraw::Plane::Perpendicular(const Vector3& v){
	// TODO: return ステートメントをここに挿入します
	if (v.x != 0.0f || v.y != 0.0f) {
		return { -v.y, v.x, 0.0f };
	} else {
		return { 0.0f, -v.z, v.y };
	}
}
