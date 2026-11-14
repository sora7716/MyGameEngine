#include "JumpCommand.h"
#include "JumpComponent.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
JumpCommand::JumpCommand() {
}

//デストラクタ
JumpCommand::~JumpCommand() {
}

//実行
void JumpCommand::Execute() {
	if(!jumpComponent_) {
		Logger::OutputLog("JumpCommandにジャンプ用のコンポーネントが設定されていません");
		return;
	}
	//ジャンプをする要求を設定
	jumpComponent_->SetJumpRequest(true);
}

//複製
std::unique_ptr<ICommand> JumpCommand::Clone() const {
	std::unique_ptr<JumpCommand>cloneInstance = std::make_unique<JumpCommand>();
	return cloneInstance;
}

//移動用コンポーネントの設定
void JumpCommand::SetJumpComponent(JumpComponent* jumpComponent) {
	jumpComponent_ = jumpComponent;
}
