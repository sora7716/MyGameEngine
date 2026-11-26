#include "LightningStrike.h"
#include "MathUtility.h"
#include "GameObject.h"
#include "BaseScene.h"
#include "Object3d.h"
#include "Logger.h"
#include "BaseCollider.h"
#include "Health.h"
#include "Enemy.h"
#include <cassert>

//コンストラクタ
LightningStrike::LightningStrike(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
LightningStrike::~LightningStrike() {
}

//初期化
void LightningStrike::Initialize() {
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
	//Object3dを取得
	object3d_ = gameObject_->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("Object3dが取得できませんでした");
		assert(false);
		return;
	}
	object3d_->SetBlendMode(BlendMode::kPremultipliedAlpha);

	//テクスチャパス
	MaterialTexturePaths defaultTexturePaths{
		.textureFilePath = "models/lightningStrike/LightningStrike_BaseColor.png",
		.environmentMap = "textures/skybox_cube.dds"
	};

	//シーンを取得
	BaseScene* scene = gameObject_->GetCurrentScene();
	//フラッシュ用のマテリアル
	scene->AddMaterialInstance("lightningStrikeMaterial",defaultTexturePaths);
	lightningStrikeMat_ = scene->GetMaterialInstance("lightningStrikeMaterial");
	//最初の色を取得
	beginColor_ = Color::GetWhite();
	//最終的なカラー
	targetColor_ = Color{1.0f,1.0f,1.0f,0.0f};
}

//更新
void LightningStrike::Update() {
	//生存時間を計測
	lifeTimer_ += mathUtility::GetDeltaTime();
	//最大で生存できる時間を過ぎたら
	if(lifeTimer_ >= maxLifetime_) {
		gameObject_->SetIsActive(false);
		//生存時間をリセット
		lifeTimer_ = 0.0f;
	}

	//色の線形補間
	Color resultColor = {};
	resultColor = Color::Lerp(beginColor_,targetColor_,lifeTimer_ / maxLifetime_);
	lightningStrikeMat_->SetParameter("color",resultColor);

	//Object3dにマテリアルをセット
	for(uint32_t i = 0; i < object3d_->GetMaterialInstances().size(); i++) {
		object3d_->SetMaterialInstance(i,lightningStrikeMat_);
	}
}

//接触した瞬間
void LightningStrike::OnTriggerEnter(BaseCollider* other) {
	GameObject* otherObject = other->GetOwner();
	if(otherObject->GetTag() == "Enemy") {
		//接触したのが敵だったら
		//HPの管理を取得
		Health* health = otherObject->GetComponent<Health>();
		if(!health) {
			Logger::OutputLog("Healthが取得できませんでした");
			assert(false);
			return;
		}
		//ダメージを与える
		health->ApplyDamage(parameter_.damage);

		//敵を取得
		Enemy* enemy = otherObject->GetComponent<Enemy>();
		if(!enemy) {
			Logger::OutputLog("Enemyが取得できませんでした");
			assert(false);
			return;
		}
		//敵にパラメータを設定
		enemy->SetShockParameter(parameter_);

	}
}

//プロパティ一覧を登録
void LightningStrike::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("生存できる最大時間",maxLifetime_);
}

//複製
std::unique_ptr<Component> LightningStrike::Clone(GameObject* gameObject) const {
	std::unique_ptr<LightningStrike>cloneInstance = std::make_unique<LightningStrike>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定の複製
	cloneInstance->SetEnabled(this->IsEnabled());
	cloneInstance->maxLifetime_ = this->maxLifetime_;

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string LightningStrike::GetTypeName() const {
	return "lightningStrike";
}

//ショックのパラメータを取得
const ShockParameter& LightningStrike::GetShockParameter() const {
	return parameter_;
}

//ショックパラメータを設定
void LightningStrike::SetShockParameter(const ShockParameter& parameter) {
	parameter_ = parameter;
}
