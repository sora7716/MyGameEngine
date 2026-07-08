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

	entityGroup_.entity[0].gameObject.transform.scale = { 5.0f,1.0f,5.0f };
	entityGroup_.entity[0].gameObject.transform.translate.y = -2.0f;

	entityGroup_.entity[1].gameObject.transform.scale = { 1.0f,1.0f,3.0f };
	entityGroup_.entity[1].gameObject.transform.translate = { -1.5f,1.0f,8.0f };

	entityGroup_.entity[2].gameObject.transform.scale = { 1.7f,0.7f,3.5f };
	entityGroup_.entity[2].gameObject.transform.eulerAngle = { -0.2f,0.0f,0.0f };
	entityGroup_.entity[2].gameObject.transform.quaternion = Quaternion::MakeQuaternionForEulerAngle(entityGroup_.entity[2].gameObject.transform.eulerAngle);
	entityGroup_.entity[2].gameObject.transform.translate = { 2.5f,3.0f,15.5f };

	entityGroup_.entity[3].gameObject.transform.scale = { 3.0f,0.5f,3.5f };
	entityGroup_.entity[3].gameObject.transform.eulerAngle = { 0.5f,0.0f,0.5f };
	entityGroup_.entity[3].gameObject.transform.quaternion = Quaternion::MakeQuaternionForEulerAngle(entityGroup_.entity[3].gameObject.transform.eulerAngle);
	entityGroup_.entity[3].gameObject.transform.translate = { 6.0f,6.2f,25.0f };

	entityGroup_.entity[4].gameObject.transform.scale = { 3.0f,0.5f,3.5f };
	entityGroup_.entity[4].gameObject.transform.eulerAngle = { 0.5f,0.0f,0.0f };
	entityGroup_.entity[4].gameObject.transform.quaternion = Quaternion::MakeQuaternionForEulerAngle(entityGroup_.entity[4].gameObject.transform.eulerAngle);
	entityGroup_.entity[4].gameObject.transform.translate = { 6.0f,8.0f,6.0f };

	entityGroup_.entity[5].gameObject.transform.translate = { 2.0f,8.5f,1.0f };

	entityGroup_.entity[6].gameObject.transform.scale = { 2.0f,2.0f,2.0f };
	entityGroup_.entity[6].gameObject.transform.translate = { -4.5f,23.5f,0.0f };
	entityGroup_.entity[6].gameObject.tag = Tag::kGoal;
}