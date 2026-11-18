#include "DefenseRotationCommand.h"
#include "Logger.h"
#include "DefenseRotationComponent.h"

//コンストラクタ
DefenseRotationCommand::DefenseRotationCommand() {
}

//デストラクタ
DefenseRotationCommand::~DefenseRotationCommand() {
}

//セットアップ
void DefenseRotationCommand::Setup(float rotateDir) {
	rotateDir_ = rotateDir;
}

//実行
void DefenseRotationCommand::Execute() {
	if(!defenseRotationComponent_) {
		Logger::OutputLog("対象を回転する用のコンポーネントが設定されていません");
		return;
	}

	//ディフェンス用のオブジェクトの回転方向を設定
	defenseRotationComponent_->SetRotateDir(rotateDir_);
	//回転を要求
	defenseRotationComponent_->SetRotationRequest(true);
}

//複製
std::unique_ptr<ICommand> DefenseRotationCommand::Clone() const {
	std::unique_ptr<DefenseRotationCommand>cloneInstance = std::make_unique<DefenseRotationCommand>();
	cloneInstance->SetDefenseRotationComponent(defenseRotationComponent_);
	return cloneInstance;
}

//ディフェンス用のオブジェクトの回転させるの設定
void DefenseRotationCommand::SetDefenseRotationComponent(DefenseRotationComponent* defenseRotationComponent) {
	defenseRotationComponent_ = defenseRotationComponent;
}
