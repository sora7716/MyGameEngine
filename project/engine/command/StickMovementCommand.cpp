#include "StickMovementCommand.h"
#include "Logger.h"
#include "MovementComponent.h"
#include <cassert>

//コンストラクタ
StickMovementCommand::StickMovementCommand() {
}

//デストラクタ
StickMovementCommand::~StickMovementCommand() {
}

//実行
void StickMovementCommand::Execute(const Vector3& axis) {
	if(!movementComponent_) {
		Logger::OutputLog("StickMovementCommandに移動用のコンポーネントが設定されていません");
		return;
	}

	//スティックの倒し具合を移動方向へ設定
	movementComponent_->AddDirection(axis);
}

//複製
std::unique_ptr<IAxisCommand> StickMovementCommand::Clone() const {
	std::unique_ptr<StickMovementCommand>cloneInstance = std::make_unique<StickMovementCommand>();
	cloneInstance->SetMovementComponent(movementComponent_);
	return cloneInstance;
}

//移動用コンポーネントの設定
void StickMovementCommand::SetMovementComponent(MovementComponent* movementComponent) {
	//移動用のコンポーネントを記録
	assert(movementComponent);
	movementComponent_ = movementComponent;
}