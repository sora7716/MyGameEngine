#include "Segment.h"
using namespace primitiveData;

//コンストラクタ
wireframe::Segment::Segment(){}

//デストラクタ
wireframe::Segment::~Segment(){}

//初期化
void wireframe::Segment::InitializeShape(){
	vertexCount_ = 2;
	indexCount_ = 2;

	//差分を設定
	segment_.diff = { 1.0f,0.0f,0.0f };
}

//更新
void wireframe::Segment::UpdateShape(){
}

//複製
std::unique_ptr<wireframe::BaseShape> wireframe::Segment::Clone() const{
	std::unique_ptr<Segment>cloneInstance = std::make_unique<Segment>();

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->segment_ = this->segment_;
	return cloneInstance;
}

//線分のゲッター
void wireframe::Segment::SetSegment(const primitiveData::Segment& segment){
	segment_ = segment;
}

//線分のゲッター
Segment wireframe::Segment::GetSegment(){
	return segment_;
}

//頂点データの設定
void wireframe::Segment::SettingVertexData(){
	//始点
	vertices_[0] = segment_.origin;

	//終点
	vertices_[1] = segment_.origin + segment_.diff;
}

//インデックスの設定
void wireframe::Segment::SettingIndexData(){
	//始点
	indices_[0] = 0;

	//終点
	indices_[1] = 1;
}
