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
	halfSize_ = { 0.5f,0.5f,0.5f };
}

//更新
void AABBCollider::Update(){
	//中心座標
	const Vector3& center = gameObject_->GetTransform().translate;

	//中心からAABBを求める
	aabb_.min = center - halfSize_;
	aabb_.max = center + halfSize_;
}

//ハーフサイズの設定
void AABBCollider::SetHalfSize(const Vector3& halfSize){
	halfSize_ = halfSize;
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

	return cloneInstance;
}

//コライダータイプの取得
ColliderType AABBCollider::GetColliderType() const{
	return ColliderType::kAABB;
}
