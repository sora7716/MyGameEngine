#include "Sphere.h"
#include "algorithms/Math.h"
#include "ImGuiManager.h"
//コンストラクタ
Sphere::Sphere() {}

//デストラクタ
Sphere::~Sphere() {}

//初期化
void Sphere::Initialize(DirectXBase* directXBase, Camera* camera) {
	vertexCount_ = kCircleVertexCount * 3;
	indexCount_ = vertexCount_ * 2;
	BaseShape::Initialize(directXBase, camera);

	//半径を設定
	sphere_.radius = 1.0f;
}

//更新
void Sphere::Update() {
	//トランスフォームに送信
	transform_.translate = sphere_.center;

	//頂点データの設定
	SettingVertexData();
	//基底クラスの更新
	BaseShape::Update();
}

//描画
void Sphere::Draw() {
	//基底クラスの描画
	BaseShape::Draw();
}

//球のセッター
void Sphere::SetSphere(const PrimitiveData::Sphere& sphere) {
	sphere_ = sphere;
}

//球のゲッター
PrimitiveData::Sphere Sphere::GetSphere() {
	return sphere_;
}

//頂点データの設定
void Sphere::SettingVertexData() {
	for (int32_t i = 0; i < kCircleVertexCount; i++) {
		float t = static_cast<float>(i) / static_cast<float>(kCircleVertexCount);
		float angle = t * Math::kPi * 2.0f;

		int32_t xy = i;
		int32_t xz = kCircleVertexCount + i;
		int32_t yz = kCircleVertexCount * 2 + i;

		//Z軸を向いている
		vertexData_[xy] = {
			std::cos(angle) * sphere_.radius,
			std::sin(angle) * sphere_.radius,
			0.0f,
			1.0f
		};

		//Y軸を向いている
		vertexData_[xz] = {
			std::cos(angle) * sphere_.radius,
			0.0f,
			std::sin(angle) * sphere_.radius,
			1.0f
		};

		//X軸を向いている
		vertexData_[yz] = {
			0.0f,
			std::cos(angle) * sphere_.radius,
			std::sin(angle) * sphere_.radius,
			1.0f
		};
	}
}

//インデックスの設定
void Sphere::SettingIndexData() {
	int32_t index = 0;

	for (int32_t circle = 0; circle < 3; circle++) {
		int32_t offset = circle * kCircleVertexCount;

		for (int32_t i = 0; i < kCircleVertexCount; i++) {
			int32_t next = (i + 1) % kCircleVertexCount;

			indexData_[index++] = offset + i;
			indexData_[index++] = offset + next;
		}
	}
}