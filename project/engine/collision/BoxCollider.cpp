#include "BoxCollider.h"
#include "GameObject.h"
#include "MatrixUtility.h"

//コンストラクタ
BoxCollider::BoxCollider(GameObject* gameObject) :BaseCollider(gameObject){
}

//デストラクタ
BoxCollider::~BoxCollider(){
}

//初期化
void BoxCollider::Initialize(){
	//基底クラスの更新
	BaseCollider::Initialize();
	halfSize_ = Vector3::GetOne() / 2.0f;

	//OBBの初期化
	obb_.Initialize();
}

//更新
void BoxCollider::Update(){
	//中心座標
	obb_.center = gameObject_->GetTransform().translate + offset_;

	//Sizeを設定
	obb_.size = halfSize_ * gameObject_->GetTransform().scale.Abs();

	//回転の設定
	matrixUtility::MakeOBBRotateMatrix(obb_.orientations, gameObject_->GetTransform().rotate);
}

//プロパティ一覧を登
void BoxCollider::RegisterProperties(PropertyRegistry* registry){
	Component::RegisterProperties(registry);
	std::function<bool()> boolGetter = [this]() -> bool{return this->IsTrigger(); };
	std::function<void(bool)> boolSetter = [this](bool isTrigger){return this->SetIsTrigger(isTrigger); };
	registry->Add("isTrigger", boolGetter, boolSetter);
	registry->Add("offset", offset_);
	registry->Add("halfSize", halfSize_);
}

//ハーフサイズの設定
void BoxCollider::SetHalfSize(const Vector3& halfSize){
	halfSize_ = halfSize;
}

//オフセットの設定
void BoxCollider::SetOffset(const Vector3& offset){
	offset_ = offset;
}

//OBBの取得
const primitiveData::OBB& BoxCollider::GetOBB(){
	return obb_;
}

//AABBの取得
primitiveData::AABB BoxCollider::GetAABB(){
	primitiveData::AABB aabb = {
		.min = obb_.center - obb_.size,
		.max = obb_.center + obb_.size
	};
	return aabb;
}

//複製
std::unique_ptr<Component> BoxCollider::Clone(GameObject* gameObject) const{
	std::unique_ptr<BoxCollider>cloneInstance = std::make_unique<BoxCollider>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetIsTrigger(this->IsTrigger());
	cloneInstance->SetBodyType(this->GetBodyType());

	return cloneInstance;
}

//軸が平行かどうかを取得
bool BoxCollider::IsAxisAligned(){
	//微小な値
	float epsilon = 0.0001f;

	//OBBの各軸を見る
	for (const Vector3& orientation : obb_.orientations){
		//軸を絶対値にする
		Vector3 axis = orientation.Abs();

		//一番でかい値を取得
		float maxComponent = axis.Max();

		if (std::abs(maxComponent - 1.0f) > epsilon){
			return false;
		}
	}

	return true;
}

//コンポーネントのタイプ名を取得
std::string BoxCollider::GetTypeName() const{
	return "BoxCollider";
}

//コライダータイプの取得
ColliderType BoxCollider::GetColliderType() const{
	return ColliderType::kBox;
}
