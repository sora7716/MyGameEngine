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
void Component::OnCollisionEnter([[maybe_unused]] const CollisionInfo& info){
	
}

//接触中
void Component::OnCollisionStay([[maybe_unused]] const CollisionInfo& info){
	
}

//離れた瞬間
void Component::OnCollisionExit([[maybe_unused]] const CollisionInfo& info){
	
}

//接触した瞬間
void Component::OnTriggerEnter([[maybe_unused]] BaseCollider* other){
	
}

//接触中
void Component::OnTriggerStay([[maybe_unused]] BaseCollider* other){
	
}

//離れた瞬間
void Component::OnTriggerExit([[maybe_unused]] BaseCollider* other){
	
}

//ゲームオブジェクトから解除する
void Component::OnGameObjectRemoving([[maybe_unused]] GameObject* target){
	
}

//更新のフェーズの取得
UpdatePhase Component::GetUpdatePhase(){
	return UpdatePhase::kMain;
}

//プロパティ一覧を登録
void Component::RegisterProperties(PropertyRegistry* registry){
	//有効フラグを登録
	registry->Add("isEnabled", isEnabled_);
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
