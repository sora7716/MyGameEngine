#include "MovementCommand.h"
#include "MovementComponent.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
MovementCommand::MovementCommand() {
}

//デストラクタ
MovementCommand::~MovementCommand() {
}

//セットアップ
void MovementCommand::Setup(const Vector3& direction) {
	//方向ベクトルの記録
	direction_ = direction;
}

//実行
void MovementCommand::Execute() {
	if(!movementComponent_) {
		Logger::OutputLog("MovementCommandに移動用のコンポーネントが設定されていません");
		return;
	}
	//移動用のコンポーネントに方向を設定
	movementComponent_->AddDirection(direction_);
}

//複製
std::unique_ptr<ICommand> MovementCommand::Clone() const {
	std::unique_ptr<MovementCommand>cloneInstance = std::make_unique<MovementCommand>();
	return cloneInstance;
}

//移動用コンポーネントの設定
void MovementCommand::SetMovementComponent(MovementComponent* movementComponent) {
	movementComponent_ = movementComponent;
}
