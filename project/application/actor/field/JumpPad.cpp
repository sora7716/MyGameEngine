#include "JumpPad.h"

//コンストラクタ
JumpPad::JumpPad() {}

//デストラクタ
JumpPad::~JumpPad() {}

//初期化
void JumpPad::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	entityGroup_.objectCount = 3;
	entityGroup_.modelName = "enemy";

	//基底クラスの初期化
	BaseGround::Initialize(object3dCommon, camera);
	//タグの変更
	for (uint32_t i = 0; i < static_cast<uint32_t>(entityGroup_.entity.size()); i++) {
		entityGroup_.entity[i].gameObject.transformData.scale = { 0.5f,0.5f,0.5f };
		entityGroup_.entity[i].gameObject.tag = Tag::kJumpPad;
	}

	entityGroup_.entity[0].gameObject.transformData.translate = { 4.5f,14.4f,1.0f };
	entityGroup_.entity[1].gameObject.transformData.translate = { 2.1f,16.3f,-1.0f };
	entityGroup_.entity[2].gameObject.transformData.translate = { 0.0f,19.7f,2.1f };
}
