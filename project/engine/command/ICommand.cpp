#include "ICommand.h"
#include <cassert>

//コンストラクタ
ICommand::ICommand() {
}

//デストラクタ
ICommand::~ICommand() {
}

//セットアップ
void ICommand::Setup(GameObject* gameObject) {
	assert(gameObject);
	gameObject_ = gameObject;
}
