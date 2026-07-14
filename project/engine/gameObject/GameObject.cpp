#include "GameObject.h"
#include "StringUtility.h"

//ゲームオブジェトの生成
std::unique_ptr<GameObject> GameObject::Create(const std::string& name){
	//インスタンスの生成
	std::unique_ptr<GameObject>instance = std::make_unique<GameObject>();
	//初期化
	instance->Initialize(name);
	return instance;
}

//コンストラクタ
GameObject::GameObject(){
}

//コピーコンストラクタ
GameObject::GameObject(const GameObject& gameObject){
	//コピーする
	*this = gameObject;
}

//デストラクタ
GameObject::~GameObject(){
}

//複製
std::unique_ptr<GameObject> GameObject::Clone() const{
	std::unique_ptr<GameObject>cloneInstance = std::make_unique<GameObject>(*this);
	return cloneInstance;
}

//初期化
void GameObject::Initialize(const std::string& name){
	name_ = name;
	transform_.Initialize();
	isActive_ = true;
	tag_ = Tag::kNone;
}

//トランスフォームの取得
Transform& GameObject::GetTransform(){
	// TODO: return ステートメントをここに挿入します
	return transform_;
}

//トランスフォームの取得
const Transform& GameObject::GetTransform() const{
	// TODO: return ステートメントをここに挿入します
	return transform_;
}

//アクティブかどうかを設定
void GameObject::SetIsActive(bool isActive){
	isActive_ = isActive;
}

//アクティブかどうかを取得
bool GameObject::IsActive() const{
	return isActive_;
}

//タグの設定
void GameObject::SetTag(Tag tag){
	tag_ = tag;
}

//タグの取得
Tag GameObject::GetTag() const{
	return tag_;
}

//名前の設定
void GameObject::SetName(const std::string& name){
	name_ = name;
}

//名前を取得
const std::string& GameObject::GetName() const{
	// TODO: return ステートメントをここに挿入します
	return name_;
}
