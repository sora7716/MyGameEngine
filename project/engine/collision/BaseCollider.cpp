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

//衝突したときの判定(押し戻しあり)
void BaseCollider::OnCollision(const CollisionInfo& info){

}

//衝突したときの判定(押し戻しなし)
void BaseCollider::OnTrigger(BaseCollider* other){
}

//めり込むかを判定するフラグの設定
void BaseCollider::SetIsTrigger(bool isTrigger){
	isTrigger_ = isTrigger;
}

//めり込むかを判定するフラグを取得
bool BaseCollider::IsTrigger(){
	return isTrigger_;
}
