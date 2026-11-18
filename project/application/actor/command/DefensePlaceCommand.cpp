#include "DefensePlaceCommand.h"
#include "Logger.h"
#include "DefensePlaceComponent.h"

//コンストラクタ
DefensePlaceCommand::DefensePlaceCommand() {
}

//デストラクタ
DefensePlaceCommand::~DefensePlaceCommand() {
}

//実行
void DefensePlaceCommand::Execute() {
	if(!defensePlaceComponent_) {
		Logger::OutputLog("DefensePlaceCommandにディフェンス用のオブジェクトを設置する機能が設定されていません");
		return;
	}
	//オブジェクトの設置を要求を設定
	defensePlaceComponent_->SetConfirmRequest(true);
}

//複製
std::unique_ptr<ICommand> DefensePlaceCommand::Clone() const {
	std::unique_ptr<DefensePlaceCommand>cloneInstance = std::make_unique<DefensePlaceCommand>();
	cloneInstance->SetDefensePlaceComponent(defensePlaceComponent_);
	return cloneInstance;
}

//ディフェンス用のオブジェクトを設置する機能の設定
void DefensePlaceCommand::SetDefensePlaceComponent(DefensePlaceComponent* defensePlaceComponent) {
	defensePlaceComponent_ = defensePlaceComponent;
}
