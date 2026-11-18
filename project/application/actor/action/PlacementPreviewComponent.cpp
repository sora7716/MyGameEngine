#include "PlacementPreviewComponent.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
PlacementPreviewComponent::PlacementPreviewComponent(GameObject* gameObject) :Component(gameObject){
}

//デストラクタ
PlacementPreviewComponent::~PlacementPreviewComponent() {
}

//初期化
void PlacementPreviewComponent::Initialize() {
	//Object3dがあるか
	object3d_ = GetOwner()->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("PlacementPreviewComponentにObject3dがコンポーネントされていません");
		return;
	}
}

//更新
void PlacementPreviewComponent::Update() {
	//表示要求がfalseなら
	if(!enableRequest_) {
		//オブジェクト3dを非表示
		object3d_->SetEnabled(false);
		return;
	}
	
	//現在のモデル名がなければ
	if(currentModelName_.empty()) {
		//オブジェクト3dを非表示
		object3d_->SetEnabled(false);
		return;
	}

	//位置を設定
	Transform& transform = GetOwner()->GetTransform();
	transform.translate = position_;

	//表示をする
	object3d_->SetEnabled(true);
}

//プロパティ一覧
void PlacementPreviewComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
}

//複製
std::unique_ptr<Component> PlacementPreviewComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<PlacementPreviewComponent>cloneInstance = std::make_unique<PlacementPreviewComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string PlacementPreviewComponent::GetTypeName() const {
	return "PlacementPreviewComponent";
}

//表示するかの要求を設定
void PlacementPreviewComponent::SetEnabledRequest(bool request) {
	enableRequest_ = request;
}

//位置の設定
void PlacementPreviewComponent::SetPosition(const Vector3& position) {
	position_ = position;
}

//表示するかの要求を取得
bool PlacementPreviewComponent::GetEnabledRequest() const {
	return enableRequest_;
}

//モデル名の設定
void PlacementPreviewComponent::SetModel(const std::string& modelName) {
	//現在のモデル名を過去のモデル名にする
	prevModelName_ = currentModelName_;
	//現在のモデル名を設定
	currentModelName_ = modelName;

	//過去と現在のモデル一覧が違う場合
	if(prevModelName_ != currentModelName_) {
		//モデルを設定
		object3d_->SetModel(currentModelName_);
	}
}
