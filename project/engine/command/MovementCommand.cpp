#include "MovementCommand.h"
//コンストラクタ
MovementCommand::MovementCommand() {
}

//デストラクタ
MovementCommand::~MovementCommand() {
}

//実行
void MovementCommand::Execute() {
}

//複製
std::unique_ptr<ICommand> MovementCommand::Clone() const {
	std::unique_ptr<MovementCommand>cloneInstance = std::make_unique<MovementCommand>();
	return cloneInstance;
}
