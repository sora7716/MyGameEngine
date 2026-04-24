#include "Line.h"

//コンストラクタ
Line::Line() {}

//デストラクタ
Line::~Line() {}

//初期化
void Line::Initialize(DirectXBase* directXBase, Camera* camera) {
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

//頂点データの設定
void Line::SettingVertexData() {
	vertexData_[0].position = segment_.origin;
	vertexData_[1].position = segment_.origin + segment_.diff;
}

//インデックスの設定
void Line::SettingIndexDate() {
	indexData_[0] = 0;
	indexData_[1] = 1;
}
