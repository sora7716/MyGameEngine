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
