#include "BaseCollider.h"

//コンストラクタ
BaseCollider::BaseCollider(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
BaseCollider::~BaseCollider(){
}

//初期化
void BaseCollider::Initialize(){
	gameObject_ = GetOwner();
}

//プロパティの一覧の登録
void BaseCollider::RegisterProperties(PropertyRegistry* registry){
	Component::RegisterProperties(registry);
	//Triggerのフラグ切り替え
	std::function<bool()> boolGetter = [this]() -> bool{return this->IsTrigger(); };
	std::function<void(bool)> boolSetter = [this](bool isTrigger){return this->SetIsTrigger(isTrigger); };
	registry->Add("isTrigger", boolGetter, boolSetter);

	//ボディタイプの切り替え
	std::function<uint32_t()> bodyTypeGetter = [this]()->uint32_t{return static_cast<uint32_t>(this->GetBodyType()); };
	std::function<void(uint32_t)> bodyTypeSetter = [this](uint32_t bodyType){return this->SetBodyType(static_cast<BodyType>(bodyType)); };
	registry->Add("BodyType", { "Static", "Dynamic" }, bodyTypeGetter, bodyTypeSetter);
}

//衝突したときの判定(押し戻しあり)
void BaseCollider::OnCollisionStay([[maybe_unused]] const CollisionInfo& info){
}

//衝突したときの判定(押し戻しなし)
void BaseCollider::OnTriggerStay([[maybe_unused]] BaseCollider* other){

}

//めり込むかを判定するフラグの設定
void BaseCollider::SetIsTrigger(bool isTrigger){
	isTrigger_ = isTrigger;
}

//ボディタイプの設定
void BaseCollider::SetBodyType(BodyType bodyType){
	bodyType_ = bodyType;
}

//めり込むかを判定するフラグを取得
bool BaseCollider::IsTrigger()const{
	return isTrigger_;
}

//ボディタイプの取得
BodyType BaseCollider::GetBodyType() const{
	return bodyType_;
}
