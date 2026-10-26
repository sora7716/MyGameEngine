#include "SphereCollider.h"

//コンストラクタ
SphereCollider::SphereCollider(GameObject* gameObject) :BaseCollider(gameObject){
}

//デストラクタ
SphereCollider::~SphereCollider(){
}

//初期化
void SphereCollider::Initialize(){
	//基底クラスの初期化
	BaseCollider::Initialize();
	sphere_ = {};
	center_ = {};
	radius_ = 1.0f;
}

//更新
void SphereCollider::Update(){
	//基底クラスの更新
	BaseCollider::Update();
	//ゲームオブジェクトを取得
	GameObject* gameObject = GetOwner();
	//SphereとGameObjectをリンクさせる
	sphere_.center = center_ + gameObject->GetTransform().translate;
	sphere_.radius = radius_ * gameObject_->GetTransform().scale.Abs().Max();
}

//プロパティ一覧の登録
void SphereCollider::RegisterProperties(PropertyRegistry* registry){
	BaseCollider::RegisterProperties(registry);
	registry->Add("center", center_);
	registry->Add("radius", radius_);
}

//複製
std::unique_ptr<Component> SphereCollider::Clone(GameObject* gameObject) const{
	std::unique_ptr<SphereCollider>cloneInstance = std::make_unique<SphereCollider>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->SetIsTrigger(this->IsTrigger());
	cloneInstance->SetBodyType(this->GetBodyType());
	cloneInstance->sphere_ = this->sphere_;
	cloneInstance->center_ = this->center_;
	cloneInstance->radius_ = this->radius_;

	return cloneInstance;
}

//コンポーネントのタイプ名を取得
std::string SphereCollider::GetTypeName() const{
	return "SphereCollider";
}

//コライダータイプの取得
ColliderType SphereCollider::GetColliderType() const{
	return ColliderType::kSphere;
}

//球の取得
const primitiveData::Sphere& SphereCollider::GetSphere() const{
	return sphere_;
}


