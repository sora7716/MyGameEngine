#include "Circle.h"
#include "MathUtility.h"
#include "GameObject.h"

//コンストラクタ
debugDraw::Circle::Circle(GameObject* gameObject) :BaseShape(gameObject){
}

//デストラクタ
debugDraw::Circle::~Circle(){}

//初期化
void debugDraw::Circle::InitializeShape(){
	vertexCount_ = 32;
	indexCount_ = vertexCount_ * 2;
	circle_.radius = 1.0f;
}

//更新
void debugDraw::Circle::UpdateShape(){
}

//複製
std::unique_ptr<Component> debugDraw::Circle::Clone(GameObject* gameObject) const{
	std::unique_ptr<Circle>cloneInstance = std::make_unique<Circle>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->circle_ = this->circle_;
	return cloneInstance;
}

//円のセッター
void debugDraw::Circle::SetCircle(const primitiveData::Circle& circle){
	circle_ = circle;
}

//円のゲッター
primitiveData::Circle debugDraw::Circle::GetCircle(){
	return circle_;
}

//頂点データの設定
void debugDraw::Circle::SettingVertexData(){
	for (uint32_t i = 0; i < vertexCount_; i++){
		float t = static_cast<float>(i) / static_cast<float>(vertexCount_);
		float angle = t * mathUtility::kPi * 2.0f;

		vertices_[i] = {
			std::cos(angle) * circle_.radius,
			std::sin(angle) * circle_.radius,
			0.0f,
			1.0f
		};
	}
}

//インデックスの設定
void debugDraw::Circle::SettingIndexData(){
	for (uint32_t i = 0; i < vertexCount_; i++){
		uint32_t next = (i + 1) % vertexCount_;

		//始点
		indices_[i * 2] = i;

		//終点
		indices_[i * 2 + 1] = next;
	}
}