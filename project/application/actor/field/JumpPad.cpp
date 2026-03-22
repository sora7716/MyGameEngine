#include "JumpPad.h"

//コンストラクタ
JumpPad::JumpPad() {
}

//デストラクタ
JumpPad::~JumpPad() {
}

//初期化
void JumpPad::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	entityGroup_.objectCount = 1;
	entityGroup_.modelName = "enemy";

	//基底クラスの初期化
	BaseGround::Initialize(object3dCommon, camera);
	//タグの変更
	for (uint32_t i = 0; i < static_cast<uint32_t>(entityGroup_.entity.size()); i++) {
		entityGroup_.entity[i].gameObject.tag = Tag::kJumpPad;
	}

	entityGroup_.entity[0].gameObject.transformData.translate = { -5.0f,0.0f,0.0f };
}
