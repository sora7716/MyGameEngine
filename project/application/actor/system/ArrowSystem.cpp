#include "ArrowSystem.h"
#include "BaseScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Arrow.h"
#include "BoxCollider.h"
#include <cassert>

//生成
std::unique_ptr<ArrowSystem> ArrowSystem::Create(BaseScene* scene) {
	std::unique_ptr<ArrowSystem>instance = std::make_unique<ArrowSystem>();
	instance->Initialize(scene);
	return instance;
}

//コンストラクタ
ArrowSystem::ArrowSystem() {
}

//デストラクタ
ArrowSystem::~ArrowSystem() {
}

//初期化
void ArrowSystem::Initialize(BaseScene* scene) {
	//シーンを記録
	assert(scene);
	scene_ = scene;
	//弓をまとめる
	GameObject* arrows = scene_->CreateGameObject();
	arrows->SetName("矢の塊");
	//弓を生成
	arrowObjects_.reserve(kArrowMaxCount);
	for(uint32_t i = 0; i < kArrowMaxCount; i++) {
		arrowObjects_.push_back(CreateArrow());
		arrowObjects_[i]->AttachTo(arrows);
	}
}

//撃つ
void ArrowSystem::Fire(const Vector3& targetPos,const Matrix4x4& headWorldMatrix) {
	//使用できるゲームオブジェクトのインデックスを設定
	uint32_t arrowIndex = 0;
	for(uint32_t i = 0; i < arrowObjects_.size(); i++) {
		if(!arrowObjects_[i]->IsActive()) {
			arrowIndex = i;
			break;
		}

		//見つからなかった場合
		if(i == arrowObjects_.size() - 1) {
			return;
		}
	}

	//矢のオブジェクトを取得
	GameObject* arrowObject = arrowObjects_[arrowIndex];
	arrowObject->SetIsActive(true);
	//矢の追加
	Arrow* arrow = arrowObject->GetComponent<Arrow>();
	Vector3 arrowPosition = Vector3(0.0f,0.5f,0.0f) * headWorldMatrix;
	arrow->Setup(targetPos,arrowPosition,attackPower_);//セットアップ
}

//攻撃力
void ArrowSystem::SetAttackPower(float power) {
	attackPower_ = power;
}

//矢の生成
GameObject* ArrowSystem::CreateArrow() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = scene_->CreateGameObject();
	gameObject->SetName("arrow");
	//タグを設定
	gameObject->SetTag("Arrow");
	//生存フラグはすべてfalse
	gameObject->SetIsActive(false);

	//Object3dを追加
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("arrow");

	//矢を設定
	gameObject->AddComponent<Arrow>();

	//Boxコライダーの追加
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetSize({0.5f,0.5f,1.0f});
	boxCollider->SetBodyType(BodyType::kDynamic);
	boxCollider->SetIsTrigger(true);

	return gameObject;
}
