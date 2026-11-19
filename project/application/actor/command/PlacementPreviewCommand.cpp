#include "PlacementPreviewCommand.h"
#include "Logger.h"
#include "PlacementPreviewComponent.h"

//コンストラクタ
PlacementPreviewCommand::PlacementPreviewCommand() {
}

//デストラクタ
PlacementPreviewCommand::~PlacementPreviewCommand() {
}

//実行
void PlacementPreviewCommand::Execute() {
	if(!placementPreviewComponent_) {
		Logger::OutputLog("置く場所の予想位置を表示する用のコンポーネントが設定されていません");
		return;
	}
	//表示非表示を切り替え
	isVisible_ = !isVisible_;

	//設置場所の表示を要求
	placementPreviewComponent_->SetEnabledRequest(isVisible_);
}

//複製
std::unique_ptr<ICommand> PlacementPreviewCommand::Clone() const {
	std::unique_ptr<PlacementPreviewCommand>cloneInstance = std::make_unique<PlacementPreviewCommand>();
	cloneInstance->SetPlacementPreviewComponent(placementPreviewComponent_);
	cloneInstance->isVisible_ = this->isVisible_;
	return cloneInstance;
}

//置く場所の予想位置を表示する用の設定
void PlacementPreviewCommand::SetPlacementPreviewComponent(PlacementPreviewComponent* placementPreviewComponent) {
	placementPreviewComponent_ = placementPreviewComponent;
}