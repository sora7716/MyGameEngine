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

//セットアップ
void PlacementPreviewComponent::Setup(Object3d* targetObject3d,const std::vector<std::string>& modelList) {
	//Object3dの記録
	assert(targetObject3d);
	targetObject3d_ = targetObject3d;
	//モデル一覧の記録
	modelList_ = modelList;
	//モデル名を設定
	modelName_ = modelList_[currentModelIndex_];
	//モデルを設定
	targetObject3d->SetModel(modelName_);
}

//初期化
void PlacementPreviewComponent::Initialize() {
}

//更新
void PlacementPreviewComponent::Update() {
	//表示要求がfalseなら
	if(!enableRequest_) {
		//オブジェクト3dを非表示
		targetObject3d_->SetEnabled(false);
		return;
	}
	//位置を設定
	Transform& transform = targetObject3d_->GetOwner()->GetTransform();
	transform.translate = position_;

	//表示をする
	targetObject3d_->SetEnabled(true);

	//オブジェクト3dがなければ
	if(!targetObject3d_) {
		Logger::OutputLog("PlacementPreviewComponentにObject3dが設定されていません");
		return;
	}

	//モデル一覧が無ければ
	if(modelList_.empty()) {
		Logger::OutputLog("PlacementPreviewComponentにモデル一覧が設定されていません");
		return;
	}

	//過去と現在のモデル一覧が違う場合
	if(prevModelIndex_ != currentModelIndex_) {
		//モデル名を取得
		modelName_ = modelList_[currentModelIndex_];
		//モデルを設定
		targetObject3d_->SetModel(modelName_);
	}
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

//モデルの検索キーの設定
void PlacementPreviewComponent::SetModelIndex(uint32_t modelIndex) {
	//今のモデルの検索キーを過去の検索キーに書き換え
	prevModelIndex_ = currentModelIndex_;
	
	//現在のモデルの検索キーを保存
	currentModelIndex_ = modelIndex;
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
