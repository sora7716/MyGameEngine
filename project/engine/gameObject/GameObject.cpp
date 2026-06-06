#include "GameObject.h"

//コンストラクタ
GameObject::GameObject() {
}

//デストラクタ
GameObject::~GameObject() {
}

//初期化
void GameObject::Initialize(const std::string& name) {
	name_ = name;
	transform_.Initialize();
	isActive_ = true;
	tag_ = Tag::kNone;
}

//トランスフォームの取得
Transform& GameObject::GetTransform() {
	// TODO: return ステートメントをここに挿入します
	return transform_;
}

//トランスフォームの取得
const Transform& GameObject::GetTransform() const {
	// TODO: return ステートメントをここに挿入します
	return transform_;
}

//
void GameObject::SetIsActive(bool isActive) {
	isActive_ = isActive;
}

//
bool GameObject::IsActive() const {
	return isActive_;
}

//タグの設定
void GameObject::SetTag(Tag tag) {
	tag_ = tag;
}

//タグの取得
Tag GameObject::GetTag() const {
	return tag_;
}

//
const std::string& GameObject::GetName() const {
	// TODO: return ステートメントをここに挿入します
	return name_;
}
