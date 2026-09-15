#include "GameObject.h"
#include "TagManager.h"
#include "StringUtility.h"
#include "Component.h"
//衝突判定のテーブルの初期化
std::vector<GameObject::NotifyOnCollision> GameObject::onCollisionTable = {
	&GameObject::NotifyOnCollisionEnter,
	&GameObject::NotifyOnCollisionStay,
	&GameObject::NotifyOnCollisionExit
};

//衝突判定のテーブルの初期化
std::vector<GameObject::NotifyOnTrigger> GameObject::onTriggerTable = {
	&GameObject::NotifyOnTriggerEnter,
	&GameObject::NotifyOnTriggerStay,
	&GameObject::NotifyOnTriggerExit
};

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

//現在の接続シーンの設定
void GameObject::SetCurrentScene(BaseScene* currentScene){
	currentScene_ = currentScene;
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

//現在の接続シーンの取得
BaseScene* GameObject::GetCurrentScene(){
	return currentScene_;
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

//衝突判定のイベントを呼び出す
void GameObject::InvokeCollisionEvent(uint32_t index, const CollisionInfo& info){
	(this->*onCollisionTable[index])(info);
}

//衝突判定のイベントを呼び出す
void GameObject::InvokeTriggerEvent(uint32_t index, BaseCollider* other){
	(this->*onTriggerTable[index])(other);
}

//接触した瞬間ということを各コンポーネントに通知する
void GameObject::NotifyOnCollisionEnter(const CollisionInfo& info){
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
		component->OnCollisionEnter(info);
	}
}

//衝突したことを各コンポーネントに通知する
void GameObject::NotifyOnCollisionStay(const CollisionInfo& info){
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
		component->OnCollisionStay(info);
	}
}

//離れた瞬間ということを各コンポーネントに通知する
void GameObject::NotifyOnCollisionExit(const CollisionInfo& info){
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
		component->OnCollisionExit(info);
	}
}

//接触した瞬間ということを各コンポーネントに通知する
void GameObject::NotifyOnTriggerEnter(BaseCollider* other){
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
		component->OnTriggerEnter(other);
	}
}

//衝突したことを各コンポーネントに通知する
void GameObject::NotifyOnTriggerStay(BaseCollider* other){
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
		component->OnTriggerStay(other);
	}
}

//離れた瞬間ということを各コンポーネントに通知する
void GameObject::NotifyOnTriggerExit(BaseCollider* other){
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
		component->OnTriggerExit(other);
	}
}
