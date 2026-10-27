#include "Plane.h"

//コンストラクタ
wireframe::Plane::Plane(){}

//デストラクタ
wireframe::Plane::~Plane(){}

//初期化
void wireframe::Plane::InitializeShape(){
	vertexCount_ = 4;
	indexCount_ = 8;
	plane_ = {
		.normal = {0.0f,1.0f,0.0f},
		.distance = 0.0f
	};
}

//更新
void wireframe::Plane::UpdateShape(){
}

//複製
std::unique_ptr<wireframe::BaseShape> wireframe::Plane::Clone() const{
	std::unique_ptr<Plane>cloneInstance = std::make_unique<Plane>();

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->plane_ = this->plane_;
	return cloneInstance;
}

//平面の設定
void wireframe::Plane::SetPlane(const primitiveData::Plane& plane){
	plane_ = plane;
}

//平面の取得
const primitiveData::Plane& wireframe::Plane::GetPlane() const{
	return plane_;
	//デストラクタ
}

//頂点データの設定
void wireframe::Plane::SettingVertexData(){
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

	for (uint32_t i = 0; i < 4; i++){
		vertices_[i] = {
			positions[i].x,
			positions[i].y,
			positions[i].z,
			1.0f
		};
	}
}

//インデックスの設定
void wireframe::Plane::SettingIndexData(){
	uint32_t indices[] = {
		//前面
		0,1,
		1,2,
		2,3,
		3,0,
	};

	//作成したインデックスデータを代入前面
	for (uint32_t i = 0; i < indexCount_; i++){
		indices_[i] = indices[i];
	}

}

//垂直の処理
Vector3 wireframe::Plane::Perpendicular(const Vector3& v){
	// TODO: return ステートメントをここに挿入します
	if (v.x != 0.0f || v.y != 0.0f){
		return { -v.y, v.x, 0.0f };
	} else{
		return { 0.0f, -v.z, v.y };
	}
}
