#include "AABBCollider.h"
#include "GameObject.h"

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
	halfSize_ = Vector3::GetOne();
}

//更新
void BoxCollider::Update(){
	//中心座標
	const Vector3& center = gameObject_->GetTransform().translate + offset_;

	//ワールドのハーフサイズを取得
	worldHalfSize_ = halfSize_ * gameObject_->GetTransform().scale.Abs();

	//中心からAABBを求める
	aabb_.min = center - worldHalfSize_;
	aabb_.max = center + worldHalfSize_;
}

//ハーフサイズの設定
void BoxCollider::SetHalfSize(const Vector3& halfSize){
	halfSize_ = halfSize;
}

//オフセットの設定
void BoxCollider::SetOffset(const Vector3& offset){
	offset_ = offset;
}

//ハーフサイズの取得
const Vector3& BoxCollider::GetHalfSize() const{
	return halfSize_;
}

//AABBの取得
const primitiveData::AABB& BoxCollider::GetAABB(){
	return aabb_;
}

//オフセットの取得
const Vector3& BoxCollider::GetOffset() const{
	return offset_;
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

//コライダータイプの取得
ColliderType BoxCollider::GetColliderType() const{
	return ColliderType::kBox;
}
