#include "GameObject.h"
#include "TagManager.h"
#include "StringUtility.h"
#include "Component.h"

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
GameObject::GameObject(const GameObject& gameObject)
	:name_(gameObject.name_),
	transform_(gameObject.transform_),
	isActive_(gameObject.isActive_),
	tag_(gameObject.tag_){
	for (const std::unique_ptr<Component>& component : gameObject.components_){
		components_.push_back(std::move(component->Clone(this)));
	}
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
	transform_ = {};
	isActive_ = true;
	tag_ = TagManager::kDefaultTagName;
}

//トランスフォームの取得
Transform& GameObject::GetTransform(){
	return transform_;
}

//トランスフォームの取得
const Transform& GameObject::GetTransform() const{
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

//名前の設定
void GameObject::SetName(const std::string& name){
	name_ = name;
}

//名前を取得
const std::string& GameObject::GetName() const{
	return name_;
}

//タグの設定
void GameObject::SetTag(const std::string& tag){
	tag_ = tag;
}

//タグの取得
const std::string& GameObject::GetTag() const{
	return tag_;
}

//コンポーネントの更新
void GameObject::UpdateComponents(UpdatePhase phase){
	//ゲームオブジェクトが有効か
	if (!isActive_){
		return;
	}

	//各コンポーネントごとに
	for (const std::unique_ptr<Component>& component : components_){
		if (!component){
			//コンポーネントが存在しているか	
			continue;

		} else if (!component->IsEnabled()){
			//コンポーネントが有効か
			continue;
		} else if (component->GetUpdatePhase() != phase){
			//設定したフェーズか
			continue;
		}

		//コンポーネントの更新
		component->Update();
	}
}

//衝突したことを各コンポーネントに通知する
void GameObject::NotifyOnCollision(const CollisionInfo& info){
	//各コンポーネントごとに
	for (const std::unique_ptr<Component>& component : components_){
		if (!component){
			//コンポーネントが存在しているか	
			continue;
		} else if (!component->IsEnabled()){
			//コンポーネントが有効か
			continue;
		}

		//コンポーネントに衝突を通知
		component->OnCollision(info);
	}
}

//衝突したことを各コンポーネントに通知する
void GameObject::NotifyOnTrigger(BaseCollider* other){
	//各コンポーネントごとに
	for (const std::unique_ptr<Component>& component : components_){
		if (!component){
			//コンポーネントが存在しているか	
			continue;
		} else if (!component->IsEnabled()){
			//コンポーネントが有効か
			continue;
		}

		//コンポーネントに衝突を通知
		component->OnTrigger(other);
	}
}
