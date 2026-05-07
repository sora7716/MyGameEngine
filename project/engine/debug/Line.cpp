#include "Line.h"

//コンストラクタ
Line::Line() {}

//デストラクタ
Line::~Line() {}

//初期化
void Line::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = 2;
	indexCount_ = 2;
	BaseShape::Initialize(directXBase, camera);
}

//更新
void Line::Update() {
	//頂点データの設定
	SettingVertexData();
	//基底クラスの更新
	BaseShape::Update();
}

//描画
void Line::Draw() {
	//基底クラスの描画
	BaseShape::Draw();
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
	indexData_[0] = 0;
	indexData_[1] = 1;
}
