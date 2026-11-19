#include "StickCameraControlCommand.h"
#include "CameraControlComponent.h"
#include "mathUtility.h"
#include "Logger.h"

//コンストラクタ
StickCameraControlCommand::StickCameraControlCommand() {
}

//デストラクタ
StickCameraControlCommand::~StickCameraControlCommand() {
}

//実行
void StickCameraControlCommand::Execute(const Vector2& axis) {
	if(!cameraControlComponent_) {
		Logger::OutputLog("StickCameraControlCommandにカメラ操作用のコンポーネントが設定されていません");
		return;
	}

	//スティックの倒し具合の軸を設定
	cameraControlComponent_->SetAxis(axis);
}

//複製
std::unique_ptr<IAxisCommand> StickCameraControlCommand::Clone() const {
	std::unique_ptr<StickCameraControlCommand>cloneInstance = std::make_unique<StickCameraControlCommand>();
	cloneInstance->SetCameraControlComponent(cameraControlComponent_);
	cloneInstance->pitch_ = this->pitch_;
	cloneInstance->yaw_ = this->yaw_;
	cloneInstance->rotSpeed_ = this->rotSpeed_;
	return cloneInstance;
}

//カメラ操作用コンポーネントの設定
void StickCameraControlCommand::SetCameraControlComponent(CameraControlComponent* cameraControlComponent) {
	cameraControlComponent_ = cameraControlComponent;
}
