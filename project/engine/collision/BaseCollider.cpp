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
	//自分の移動係数
	float myMoveFactor = 0.0f;
	if (bodyType_ == BodyType::kDynamic){
		myMoveFactor = 1.0f;
	}

	//対象の移動係数
	float otherMoveFactor = 0.0f;
	if (info.other->GetBodyType() == BodyType::kDynamic){
		otherMoveFactor = 1.0f;
	}

	//移動係数の合計
	float moveFactorSum = myMoveFactor + otherMoveFactor;

	//static同士だった場合
	if (moveFactorSum <= 0.0f){
		return;
	}

	//自分のTransformを取得
	Transform& transform = GetOwner()->GetTransform();
	//自分の移動係数の割合を取得
	float rate = myMoveFactor / moveFactorSum;
	//押し戻しをする
	transform.translate += info.normal * info.penetrationDepth * rate;
}

//衝突したときの判定(押し戻しなし)
void BaseCollider::OnTrigger(BaseCollider* other){
	(void)other;
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
