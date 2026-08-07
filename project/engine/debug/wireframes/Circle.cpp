#include "Circle.h"
#include "MathUtility.h"
using namespace debugDraw;

//コンストラクタ
Circle::Circle() {}

//デストラクタ
Circle::~Circle() {}

//初期化
void Circle::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = 32;
	indexCount_ = vertexCount_ * 2;
	BaseShape::Initialize(directXBase, camera);

	circle_.radius = 1.0f;
}

//更新
void Circle::Update() {
	//トランスフォームに送信
	transform_.quaternion = Quaternion::MakeQuaternionForEulerAngle(circle_.eulerAngle);
	transform_.translate = circle_.center;

	//基底クラスの更新
	BaseShape::Update();
}

//円のセッター
void Circle::SetCircle(const primitiveData::Circle& circle) {
	circle_ = circle;
}

//円のゲッター
primitiveData::Circle Circle::GetCircle() {
	return circle_;
}

//頂点データの設定
void Circle::SettingVertexData() {
	for (int32_t i = 0; i < vertexCount_; i++) {
		float t = static_cast<float>(i) / static_cast<float>(vertexCount_);
		float angle = t * mathUtility::kPi * 2.0f;

		vertexData_[i] = {
			std::cos(angle) * circle_.radius,
			std::sin(angle) * circle_.radius,
			0.0f,
			1.0f
		};
	}
}

//インデックスの設定
void Circle::SettingIndexData() {
	for (int32_t i = 0; i < vertexCount_; i++) {
		int32_t next = (i + 1) % vertexCount_;

		//始点
		indexData_[i * 2] = i;

		//終点
		indexData_[i * 2 + 1] = next;
	}
}