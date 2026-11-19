#include "PlacementSwitchComponent.h"
#include "Logger.h"

//コンストラクタ
PlacementSwitchComponent::PlacementSwitchComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
PlacementSwitchComponent::~PlacementSwitchComponent() {
}

//セットアップ
void PlacementSwitchComponent::Setup(const std::vector<std::string>& modelNameList) {
	//モデル名リストを取得
	modelNameList_ = modelNameList;
}

//初期化
void PlacementSwitchComponent::Initialize() {
}

//更新
void PlacementSwitchComponent::Update() {
	//切り替えフラグをリセット
	isSwitched_ = false;

	//実行できない場合
	if(!isRunning_) {
		return;
	}

	//切り替え要求がない場合
	if(!switchRequest_) {
		return;
	}
	//リクエストをリセット
	switchRequest_ = false;

	//リストが空の場合
	if(modelNameList_.empty()) {
		Logger::OutputLog("モデル名リストがPlacementSwitchComponentに設定されていません");
		return;
	}

	//モデルを切り替える方向が設定されていない場合
	if(dir_ == 0) {
		Logger::OutputLog("モデルを切り替える方向がPlacementSwitchComponentに設定されていません");
		return;
	}

	//新しい検索キー
	int32_t newIndex = currentIndex_ + dir_;

	//新しい検索キーの中身を確認
	if(newIndex < 0) {
		//0より小さい場合
		newIndex = static_cast<int32_t>(modelNameList_.size() - 1);
	} else if(newIndex >= static_cast<int32_t>(modelNameList_.size())) {
		//要素数以上になった場合
		newIndex = 0;
	}

	//現在のインデックスを切り替える
	currentIndex_ = newIndex;

	//切り替えたことを記録
	isSwitched_ = true;
}

//プロパティ一覧
void PlacementSwitchComponent::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
}

//複製
std::unique_ptr<Component> PlacementSwitchComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<PlacementSwitchComponent>cloneInstance = std::make_unique<PlacementSwitchComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string PlacementSwitchComponent::GetTypeName() const {
	return "PlacementSwitchComponent";
}

//実行するか設定
void PlacementSwitchComponent::SetIsRunning(bool isRunning) {
	isRunning_ = isRunning;
}

//切り替え要求を設定
void PlacementSwitchComponent::SetSwitchRequest(bool request) {
	switchRequest_ = request;
}

//切り替える方向を設定
void PlacementSwitchComponent::SetSwitchDir(int32_t dir) {
	dir_ = dir;
}

//現在のモデル名を取得
const std::string& PlacementSwitchComponent::GetCurrentModelName() const {
	return modelNameList_[currentIndex_];
}

//切り替えたことを通知する用のフラグを取得
bool PlacementSwitchComponent::IsSwitched() const {
	return isSwitched_;
}
