#include "ThunderTower.h"
#include "GameObject.h"
#include "Object3d.h"
#include "MaterialInstance.h"
#include "Logger.h"
#include "BaseScene.h"
#include "ImGuiManager.h"
#include "MathUtility.h"
#include "BaseCollider.h"
#include "LightningStrikeSystem.h"
#include <cassert>
#include <array>

//コンストラクタ
ThunderTower::ThunderTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
ThunderTower::~ThunderTower() {
}

//セットアップ
void ThunderTower::Setup(LightningStrikeSystem* lightningStrikeSystem) {
	assert(lightningStrikeSystem);
	lightningStrikeSystem_ = lightningStrikeSystem;
}

//初期化
void ThunderTower::Initialize() {
	//ゲームオブジェクトを取得
	gameObject_ = GetOwner();
	//Object3dの取得
	object3d_ = gameObject_->GetComponent<Object3d>();
	if(!object3d_) {
		Logger::OutputLog("Object3dが取得できませんでした");
		assert(false);
		return;
	}
	//ブレンドモードを切り替え
	object3d_->SetBlendMode(BlendMode::kPremultipliedAlpha);

	//マテリアルの生成
	BaseScene* scene = gameObject_->GetCurrentScene();
	//デフォルトのテクスチャパス
	MaterialTexturePaths defaultTexturePaths{
		.textureFilePath = "models/thunderTower/ThunderTower_BaseColor.png",
		.environmentMap = "textures/skybox_cube.dds"
	};
	//デフォルト
	scene->AddMaterialInstance("thunderTowerDefaultMat",defaultTexturePaths);
	defaultMat_ = scene->GetMaterialInstance("thunderTowerDefaultMat");

	//フラッシュ
	scene->AddMaterialInstance("thunderTowerFlashMat");
	MaterialInstance* thunderTowerFlashMat = scene->FindMaterialInstance("thunderTowerFlashMat");
	thunderTowerFlashMat->SetParameter("color",Color{0.82f,0.70f,1.00f,0.0f});
	thunderTowerFlashMat->SetParameter("enabledLighting",uint32_t{0});
	flashMat_ = scene->GetMaterialInstance("thunderTowerFlashMat");
}

//デバッグでImGuiを使えるようにする
void ThunderTower::DebugImGui() {
	ImGui::Begin("ThunderTower");
	ImGui::Text("メッシュの数:%d",object3d_->GetMeshDataSize());
	ImGui::End();
}

//更新
void ThunderTower::Update() {
	//振る舞いの要求があったら
	if(behaviorRequest_ != Behavior::kCount) {
		//要求をコピー
		behavior_ = behaviorRequest_;
		//要求をリセット
		behaviorRequest_ = Behavior::kCount;

		//要求が攻撃だった場合
		if(behavior_ == Behavior::kAttack) {
			//初期化
			InitializeAttack();
		}
	}

	//振る舞いが攻撃だった場合
	if(behavior_ == Behavior::kAttack) {
		//更新
		UpdateAttack();
	}
}

//接触した瞬間
void ThunderTower::OnTriggerEnter(BaseCollider* other) {
	GameObject* otherObject = other->GetOwner();
	if(otherObject->GetTag() == "Enemy") {
		targets_.push_back(otherObject);
	}
}

//接触中
void ThunderTower::OnTriggerStay(BaseCollider* other) {
	GameObject* otherObject = other->GetOwner();
	if(otherObject->GetTag() == "Enemy") {
		//敵だった場合
		if(behavior_ == Behavior::kIdol) {
			//待機だった場合
			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

//離れた瞬間
void ThunderTower::OnTriggerExit(BaseCollider* other) {
	GameObject* otherObject = other->GetOwner();
	if(otherObject->GetTag() == "Enemy") {
		//離れたオブジェクトを探索
		std::vector<GameObject*>::iterator it = std::find(targets_.begin(),targets_.end(),otherObject);
		//はなればオブジェクトがいたら
		if(it != targets_.end()) {
			//配列から消去
			targets_.erase(it);
		}
	}
}

//プロパティ一覧
void ThunderTower::RegisterProperties(PropertyRegistry* registry) {
	Component::RegisterProperties(registry);
	registry->Add("攻撃のインターバル",attackInterval_);
}

//複製
std::unique_ptr<Component> ThunderTower::Clone(GameObject* gameObject) const {
	std::unique_ptr<ThunderTower>cloneInstance = std::make_unique<ThunderTower>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//設定を複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を返す
std::string ThunderTower::GetTypeName() const {
	return "ThunderTower";
}

//攻撃の初期化
void ThunderTower::InitializeAttack() {
	//攻撃の計測タイマーのリセット
	attackTimer_ = 0.0f;
}

//攻撃の更新
void ThunderTower::UpdateAttack() {
	//時間を計測
	attackTimer_ += mathUtility::GetDeltaTime();

	//光らせる場所
	ChargePart flashPart = ChargePart::kCount;

	//各チャージの重み
	std::array<float,3>chargeWeights = {3.0f,1.5f,0.3f};
	//重みの合計
	float chargeSum = chargeWeights[0] + chargeWeights[1] + chargeWeights[2];
	//下の光らせる時間
	if(attackTimer_ < attackInterval_ * chargeWeights[0] / chargeSum) {
		flashPart = ChargePart::kBottom;
	} else if(attackTimer_ < attackInterval_ * (chargeWeights[0] + chargeWeights[1]) / chargeSum) {
		flashPart = ChargePart::kMiddle;
	} else if(attackTimer_ < attackInterval_ * (chargeSum) / chargeSum) {
		flashPart = ChargePart::kTop;
	} else {
		//攻撃対象の配列すべてにアクセス
		for(GameObject* target : targets_) {
			

			//落雷を生成
			lightningStrikeSystem_->Strike(target->GetWorldPos());
		}
		//リクエストを待機状態に戻す
		behaviorRequest_ = Behavior::kIdol;
	}

	//マテリアルの変更
	for(uint32_t i = 0; i < static_cast<uint32_t>(ChargePart::kCount); i++) {
		//検索キーと一致した場所を光らせる
		const bool isFlash = i == static_cast<uint32_t>(flashPart);
		if(isFlash) {
			//光らせるマテリアルを適応
			object3d_->SetMaterialInstance(i,flashMat_);
		} else {
			//光らせないマテリアルを適応
			object3d_->SetMaterialInstance(i,defaultMat_);
		}
	}
}