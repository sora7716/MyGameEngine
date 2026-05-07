#include "Cube.h"
//コンストラクタ
Cube::Cube() {}

//デストラクタ
Cube::~Cube() {}

//初期化
void Cube::Initialize(DirectXBase* directXBase, Camera* camera) {
	BaseShape::Initialize(directXBase, camera);
}

//更新
void Cube::Update() {
	//頂点データの設定
	SettingVertexData();
	//基底クラスの更新
	BaseShape::Update();
}

//デバッグ
void Cube::Debug() {}

//描画
void Cube::Draw() {
	//基底クラスの描画
	BaseShape::Draw();
}

//頂点データの設定
void Cube::SettingVertexData() {
	vertexData_[0].position = segment_.origin;
	vertexData_[1].position = segment_.origin + segment_.diff;
}

//インデックスの設定
void Cube::SettingIndexData() {
	indexData_[0] = 0;
	indexData_[1] = 1;
}