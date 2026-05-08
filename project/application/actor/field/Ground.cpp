#include "Ground.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "Object3dCommon.h"
#include "ImGuiManager.h"

//コンストラクタ
Ground::Ground() {}

//デストラクタ
Ground::~Ground() {}

//初期化
void Ground::Initialize(Object3dCommon* object3dCommon, Camera* camera) {
	entityGroup_.objectCount = 7;
	entityGroup_.modelName = "ground";
	
	//基底クラスの初期化
	BaseGround::Initialize(object3dCommon, camera);

	entityGroup_.entity[0].gameObject.transformData.scale = { 5.0f,1.0f,5.0f };
	entityGroup_.entity[0].gameObject.transformData.translate.y = -2.0f;

	entityGroup_.entity[1].gameObject.transformData.scale = { 1.0f,1.0f,3.0f };
	entityGroup_.entity[1].gameObject.transformData.translate = { -1.5f,1.0f,8.0f };

	entityGroup_.entity[2].gameObject.transformData.scale = { 1.7f,0.7f,3.5f };
	entityGroup_.entity[2].gameObject.transformData.eulerAngle = { -0.2f,0.0f,0.0f };
	entityGroup_.entity[2].gameObject.transformData.quaternion = Quaternion::MakeQuaternionForEulerAngle(entityGroup_.entity[2].gameObject.transformData.eulerAngle);
	entityGroup_.entity[2].gameObject.transformData.translate = { 2.5f,3.0f,15.5f };

	entityGroup_.entity[3].gameObject.transformData.scale = { 3.0f,0.5f,3.5f };
	entityGroup_.entity[3].gameObject.transformData.eulerAngle = { 0.5f,0.0f,0.5f };
	entityGroup_.entity[3].gameObject.transformData.quaternion = Quaternion::MakeQuaternionForEulerAngle(entityGroup_.entity[3].gameObject.transformData.eulerAngle);
	entityGroup_.entity[3].gameObject.transformData.translate = { 6.0f,6.2f,25.0f };

	entityGroup_.entity[4].gameObject.transformData.scale = { 3.0f,0.5f,3.5f };
	entityGroup_.entity[4].gameObject.transformData.eulerAngle = { 0.5f,0.0f,0.0f };
	entityGroup_.entity[4].gameObject.transformData.quaternion = Quaternion::MakeQuaternionForEulerAngle(entityGroup_.entity[4].gameObject.transformData.eulerAngle);
	entityGroup_.entity[4].gameObject.transformData.translate = { 6.0f,8.0f,6.0f };

	entityGroup_.entity[5].gameObject.transformData.translate = { 2.0f,8.5f,1.0f };

	entityGroup_.entity[6].gameObject.transformData.scale = { 2.0f,2.0f,2.0f };
	entityGroup_.entity[6].gameObject.transformData.translate = { -4.5f,23.5f,0.0f };
	entityGroup_.entity[6].gameObject.tag = Tag::kGoal;
}