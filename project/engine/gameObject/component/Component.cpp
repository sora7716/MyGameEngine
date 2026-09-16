#include "Component.h"

//デストラクタ
Component::~Component(){
}

//初期化
void Component::Initialize(){
}

//更新
void Component::Update(){
}

//デバッグでImGuiを使用できるようにする
void Component::DebugImGui(){
}

//接触した瞬間
void Component::OnCollisionEnter(const CollisionInfo& info){
	(void)info;
}

//接触中
void Component::OnCollisionStay(const CollisionInfo& info){
	(void)info;
}

//離れた瞬間
void Component::OnCollisionExit(const CollisionInfo& info){
	(void)info;
}

//接触した瞬間
void Component::OnTriggerEnter(BaseCollider* other){
	(void)other;
}

//接触中
void Component::OnTriggerStay(BaseCollider* other){
	(void)other;
}

//離れた瞬間
void Component::OnTriggerExit(BaseCollider* other){
	(void)other;
}

//更新のフェーズの取得
UpdatePhase Component::GetUpdatePhase(){
	return UpdatePhase::kMain;
}

//取り付け先を取得
GameObject* Component::GetOwner() const{
	return owner_;
}

//有効状態の設定
void Component::SetEnabled(bool isEnabled){
	isEnabled_ = isEnabled;
}

//有効状態の取得
bool Component::IsEnabled() const{
	return isEnabled_;
}

//コンストラクタ
Component::Component(GameObject* owner) :owner_(owner){
}
