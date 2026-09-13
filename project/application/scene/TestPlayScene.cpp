#include "TestPlayScene.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Model.h"
#include "Mesh.h"
#include "Collision.h"
#include "SkyBox.h"
#include "Sprite.h"
#include "Cube.h"
#include "Circle.h"
#include "Plane.h"
#include "Line.h"
#include "Sphere.h"
#include "Frustum.h"
#include "ParticleSystem.h"
#include "ModelManager.h"
#include "LightingManager.h"
#include <numbers>

//コンストラクタ
TestPlayScene::TestPlayScene(){};

//デストラクタ
TestPlayScene::~TestPlayScene(){};

//初期化
void TestPlayScene::Initialize(){
	//基底クラスの初期化
	BaseScene::Initialize();
}

//デバッグ
void TestPlayScene::Debug(){
	//基底クラスのデバッグ
	BaseScene::Debug();
#ifdef USE_IMGUI
#endif // USE_IMGUI
}
