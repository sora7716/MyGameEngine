#include "AABBCollider.h"
#include "GameObject.h"

//コンストラクタ
AABBCollider::AABBCollider(GameObject* gameObject) :BaseCollider(gameObject){
}

//デストラクタ
AABBCollider::~AABBCollider(){
}

//初期化
void AABBCollider::Initialize(){
	//基底クラスの更新
	BaseCollider::Initialize();
	halfSize_ = Vector3::GetAllOne();
}

//更新
void AABBCollider::Update(){
	//中心座標
	const Vector3& center = gameObject_->GetTransform().translate;

	//ワールドのハーフサイズを取得
	worldHalfSize_ = halfSize_ * gameObject_->GetTransform().scale.Abs();

	//中心からAABBを求める
	aabb_.min = center - worldHalfSize_;
	aabb_.max = center + worldHalfSize_;
}

//ハーフサイズの設定
void AABBCollider::SetHalfSize(const Vector3& halfSize){
	halfSize_ = halfSize;
}

//ハーフサイズの取得
const Vector3& AABBCollider::GetHalfSize() const{
	return halfSize_;
}

//AABBの取得
const primitiveData::AABB& AABBCollider::GetAABB(){
	return aabb_;
}

//複製
std::unique_ptr<Component> AABBCollider::Clone(GameObject* gameObject) const{
	std::unique_ptr<AABBCollider>cloneInstance = std::make_unique<AABBCollider>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetIsTrigger(this->IsTrigger());
	cloneInstance->SetBodyType(this->GetBodyType());

	return cloneInstance;
}

//コライダータイプの取得
ColliderType AABBCollider::GetColliderType() const{
	return ColliderType::kAABB;
}
