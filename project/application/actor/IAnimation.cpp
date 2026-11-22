#include "IAnimation.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Logger.h"
#include <cassert>

//セットアップ
void IAnimation::Setup(GameObject* gameObject) {
	//ゲームオブジェクトの記録
	assert(gameObject);
	gameObject_ = gameObject;
	//Object3dの取得
	object3d_ = gameObject_->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("Object3dがAnimaitonに設定されていません");
		assert(false);
	}
}
