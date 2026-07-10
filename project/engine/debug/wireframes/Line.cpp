#include "Line.h"
using namespace primitiveData;
using namespace Primitive;


//コンストラクタ
Line::Line() {}

//デストラクタ
Line::~Line() {}

//初期化
void Line::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = 2;
	indexCount_ = 2;
	BaseShape::Initialize(directXBase, camera);

	//差分を設定
	segment_.diff = { 1.0f,0.0f,0.0f };
}

//更新
void Line::Update() {
	//基底クラスの更新
	BaseShape::Update();
}

//線分のゲッター
void Line::SetSegment(const Segment& segment) {
	segment_ = segment;
}

//線分のゲッター
Segment Line::GetSegment() {
	return segment_;
}

//頂点データの設定
void Line::SettingVertexData() {
	//始点
	vertexData_[0].position = segment_.origin;
	vertexData_[0].texcoord = { 0.0f,0.0f };
	vertexData_[0].normal = { 0.0f,0.0f,1.0f };

	//終点
	vertexData_[1].position = segment_.origin + segment_.diff;
	vertexData_[1].texcoord = { 1.0f,0.0f };
	vertexData_[1].normal = { 0.0f,0.0f,1.0f };
}

//インデックスの設定
void Line::SettingIndexData() {
	//始点
	indexData_[0] = 0;

	//終点
	indexData_[1] = 1;
}
