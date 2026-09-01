#include "Sphere.h"
#include "MathUtility.h"
#include "ImGuiManager.h"
#include "GameObject.h"
using namespace debugDraw;

//コンストラクタ
debugDraw::Sphere::Sphere(GameObject* gameObject) :BaseShape(gameObject){
}

//デストラクタ
Sphere::~Sphere(){}

//初期化
void Sphere::InitializeShape(){
	vertexCount_ = kCircleVertexCount * 3;
	indexCount_ = vertexCount_ * 2;
	//半径を設定
	sphere_.radius = 1.0f;
}

//更新
void Sphere::UpdateShape(){
	GameObject* gameObject = GetOwner();
	sphere_.center = gameObject->GetTransform().translate;
	sphere_.radius = gameObject->GetTransform().scale.Max();
}

//複製
std::unique_ptr<Component> debugDraw::Sphere::Clone(GameObject* gameObject) const{
	std::unique_ptr<Sphere>cloneInstance = std::make_unique<Sphere>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//基底クラスの内容コピー
	CopyBaseSetting(*cloneInstance);

	//内容をコピー
	cloneInstance->sphere_ = this->sphere_;
	return cloneInstance;
}

//球のセッター
void Sphere::SetSphere(const primitiveData::Sphere& sphere){
	sphere_ = sphere;
}

//球のゲッター
primitiveData::Sphere Sphere::GetSphere(){
	return sphere_;
}

//頂点データの設定
void Sphere::SettingVertexData(){
	for (int32_t i = 0; i < kCircleVertexCount; i++){
		float t = static_cast<float>(i) / static_cast<float>(kCircleVertexCount);
		float angle = t * mathUtility::kPi * 2.0f;

		int32_t xy = i;
		int32_t xz = kCircleVertexCount + i;
		int32_t yz = kCircleVertexCount * 2 + i;

		//Z軸を向いている
		vertices_[xy] = {
			std::cos(angle) * sphere_.radius,
			std::sin(angle) * sphere_.radius,
			0.0f,
			1.0f
		};

		//Y軸を向いている
		vertices_[xz] = {
			std::cos(angle) * sphere_.radius,
			0.0f,
			std::sin(angle) * sphere_.radius,
			1.0f
		};

		//X軸を向いている
		vertices_[yz] = {
			0.0f,
			std::cos(angle) * sphere_.radius,
			std::sin(angle) * sphere_.radius,
			1.0f
		};
	}
}

//インデックスの設定
void Sphere::SettingIndexData(){
	int32_t index = 0;

	for (int32_t circle = 0; circle < 3; circle++){
		int32_t offset = circle * kCircleVertexCount;

		for (int32_t i = 0; i < kCircleVertexCount; i++){
			int32_t next = (i + 1) % kCircleVertexCount;

			indices_[index++] = offset + i;
			indices_[index++] = offset + next;
		}
	}
}