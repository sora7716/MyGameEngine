#include "PlacementSwitchCommand.h"
#include "Logger.h"
#include "PlacementSwitchComponent.h"

//コンストラクタ
PlacementSwitchCommand::PlacementSwitchCommand() {
}

//デストラクタ
PlacementSwitchCommand::~PlacementSwitchCommand() {
}

//セットアップ
void PlacementSwitchCommand::Setup(int32_t dir) {
	//切り替える方向を設定
	dir_ = dir;
}

//実行
void PlacementSwitchCommand::Execute() {
	if(!placementSwitchComponent_) {
		Logger::OutputLog("PlacementSwitchCommandに設置モデルを切り替える用のコンポーネントが設定されていません");
		return;
	}
	//切り替え方向を設定
	placementSwitchComponent_->SetSwitchDir(dir_);
	//設置モデルの切り替えを要求
	placementSwitchComponent_->SetSwitchRequest(true);
}

//複製
std::unique_ptr<ICommand> PlacementSwitchCommand::Clone() const {
	std::unique_ptr<PlacementSwitchCommand>cloneInstance = std::make_unique<PlacementSwitchCommand>();
	cloneInstance->Setup(dir_);
	cloneInstance->SetPlacementSwitchComponent(placementSwitchComponent_);
	return cloneInstance;
}

//設置モデルを切り替えるの設定
void PlacementSwitchCommand::SetPlacementSwitchComponent(PlacementSwitchComponent* placementSwitchComponent) {
	placementSwitchComponent_ = placementSwitchComponent;
}
