#include "BaseDefenseObject.h"
#include "BoxCollider.h"

//コンストラクタ
BaseDefenseObject::BaseDefenseObject(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
BaseDefenseObject::~BaseDefenseObject() {
}

//複製
std::unique_ptr<Component> BaseDefenseObject::Clone(GameObject* gameObject) const {
	std::unique_ptr<BaseDefenseObject>cloneInstance = std::make_unique<BaseDefenseObject>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//更新
void BaseDefenseObject::Update() {
}

//接触中
void BaseDefenseObject::OnTriggerStay(BaseCollider* other) {
	if(other->GetOwner()->GetTag() == "DefenseObject") {
		isOnTriggerStay_ = true;
	}
}

//離れた瞬間
void BaseDefenseObject::OnTriggerExit(BaseCollider* other) {
	(void)other;
	//重なっていたらのフラグのリセット
	isOnTriggerStay_ = false;
}

//コンポーネントのタイプ名を返す
std::string BaseDefenseObject::GetTypeName() const {
	return "BaseDefenseObject";
}

//重なっていたらを取得
bool BaseDefenseObject::IsOnTriggerStay() const {
	return isOnTriggerStay_;
}
