#include "LightningStrikeSystem.h"
#include "BaseScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "LightningStrike.h"
#include "BoxCollider.h"
#include <cassert>

//生成
std::unique_ptr<LightningStrikeSystem> LightningStrikeSystem::Create(BaseScene* scene) {
	std::unique_ptr<LightningStrikeSystem>instance = std::make_unique<LightningStrikeSystem>();
	instance->Initialize(scene);
	return instance;
}

//コンストラクタ
LightningStrikeSystem::LightningStrikeSystem() {
}

//デストラクタ
LightningStrikeSystem::~LightningStrikeSystem() {
}

//初期化
void LightningStrikeSystem::Initialize(BaseScene* scene) {
	//シーンを記録
	assert(scene);
	scene_ = scene;

	//落雷をまとめる
	GameObject* lightningStrikes = scene_->CreateGameObject();
	lightningStrikes->SetName("落雷の塊");
	//落雷を生成
	lightningStrikeObjects_.reserve(kLightningStrikeMaxCount);
	for(uint32_t i = 0; i < kLightningStrikeMaxCount; i++) {
		lightningStrikeObjects_.push_back(CreateLightningStrike());
		lightningStrikeObjects_[i]->AttachTo(lightningStrikes);
	}
}

//落雷する
void LightningStrikeSystem::Strike(const Vector3& targetPos) {
	//使用できるゲームオブジェクトのインデックスを設定
	uint32_t lightningStrikeIndex = 0;
	for(uint32_t i = 0; i < lightningStrikeObjects_.size(); i++) {
		if(!lightningStrikeObjects_[i]->IsActive()) {
			lightningStrikeIndex = i;
			break;
		}

		//見つからなかった場合
		if(i == lightningStrikeObjects_.size() - 1) {
			return;
		}
	}

	//落雷のオブジェクトを取得
	GameObject* lightningStrikeObject = lightningStrikeObjects_[lightningStrikeIndex];
	lightningStrikeObject->SetIsActive(true);
	//位置を設定
	lightningStrikeObject->GetTransform().translate = targetPos;

	//落雷を取得
	LightningStrike* lightningStrike = lightningStrikeObject->GetComponent<LightningStrike>();
	//パラメータを設定
	lightningStrike->SetShockParameter(parameter_);
}

//ショックパラメータを設定
void LightningStrikeSystem::SetShockParameter(const ShockParameter& parameter) {
	parameter_ = parameter;
}

//落雷の生成
GameObject* LightningStrikeSystem::CreateLightningStrike() {
	//ゲームオブジェクトを生成
	GameObject* gameObject = scene_->CreateGameObject();
	//非アクティブにする
	gameObject->SetIsActive(false);
	//名前を設定
	gameObject->SetName("落雷");
	//タグを設定
	gameObject->SetTag("LightningStrike");

	//Object3dを追加
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("lightningStrike");
	object3d->SetRenderTransformMode(RenderTransformMode::kBillboard);

	//落雷を追加
	gameObject->AddComponent<LightningStrike>();

	//Boxコライダーの追加
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	boxCollider->SetBodyType(BodyType::kDynamic);
	boxCollider->SetIsTrigger(true);
	boxCollider->SetOffset({0.0f,1.0f,0.0f});
	boxCollider->SetSize({1.0f,2.5f,1.0f});

	return gameObject;
}
