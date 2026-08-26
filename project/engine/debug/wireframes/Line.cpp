#include "Line.h"
using namespace primitiveData;
using namespace debugDraw;


//コンストラクタ
debugDraw::Line::Line(GameObject* gameObject) :BaseShape(gameObject){
}

//デストラクタ
Line::~Line(){}

//初期化
void Line::InitializeShape(){
	vertexCount_ = 2;
	indexCount_ = 2;

	//差分を設定
	segment_.diff = { 1.0f,0.0f,0.0f };
}

//更新
void Line::UpdateShape(){
}

//複製
std::unique_ptr<Component> debugDraw::Line::Clone(GameObject* gameObject) const{
	std::unique_ptr<Line>cloneInstance = std::make_unique<Line>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->segment_ = this->segment_;
	return cloneInstance;
}

//線分のゲッター
void Line::SetSegment(const Segment& segment){
	segment_ = segment;
}

//線分のゲッター
Segment Line::GetSegment(){
	return segment_;
}

//頂点データの設定
void Line::SettingVertexData(){
	//始点
	vertices_[0] = segment_.origin;

	//終点
	vertices_[1] = segment_.origin + segment_.diff;
}

//インデックスの設定
void Line::SettingIndexData(){
	//始点
	indices_[0] = 0;

	//終点
	indices_[1] = 1;
}
