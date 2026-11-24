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

//接触した瞬間
void BaseDefenseObject::OnTriggerEnter(BaseCollider* other) {
	if(other->GetOwner()->GetTag() == "DefenseObject") {
		overlapCount_++;
	}
}

//離れた瞬間
void BaseDefenseObject::OnTriggerExit(BaseCollider* other) {
	if(other->GetOwner()->GetTag() == "DefenseObject") {
		//カウントに値が入っていたら
		if(overlapCount_ > 0) {
			//衝突判定を抜けたら
			overlapCount_--;
		}
	}
}

//コンポーネントのタイプ名を返す
std::string BaseDefenseObject::GetTypeName() const {
	return "BaseDefenseObject";
}

//設置物と重なっているを取得
bool BaseDefenseObject::IsOverlappingDefense() const {
	return overlapCount_ > 0;
}
