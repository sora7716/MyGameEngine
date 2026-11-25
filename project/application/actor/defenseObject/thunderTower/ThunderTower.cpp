#include "ThunderTower.h"
#include "GameObject.h"
#include "Object3d.h"
#include "MaterialInstance.h"
#include "Logger.h"
#include "BaseScene.h"
#include "ImGuiManager.h"
#include "MathUtility.h"
#include "BaseCollider.h"
#include "LightningStrike.h"
#include <cassert>

//コンストラクタ
ThunderTower::ThunderTower(GameObject* gameObject) :BaseDefenseObject(gameObject) {
}

//デストラクタ
ThunderTower::~ThunderTower() {
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

	//ステップを計算
	float step = attackInterval_ / static_cast<float>(ChargePart::kCount);

	//光らせる場所
	ChargePart flashPart = ChargePart::kCount;

	//下の光らせる時間
	if(attackTimer_ < step) {
		flashPart = ChargePart::kBottom;
	} else if(attackTimer_ < step * 2.0f) {
		flashPart = ChargePart::kMiddle;
	} else if(attackTimer_ < step * 3.0f) {
		flashPart = ChargePart::kTop;
	} else {
		//攻撃対象の配列すべてにアクセス
		for(GameObject* target : targets_) {
			//落雷を生成
			CreateLightningStrike(target->GetWorldPos());
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
			break;
		}
		//光らせないマテリアルを適応
		object3d_->SetMaterialInstance(i,defaultMat_);
	}
}

//落雷の生成
void ThunderTower::CreateLightningStrike(const Vector3& spawnPos) {
	//シーンを取得
	BaseScene* scene = gameObject_->GetCurrentScene();
	//ゲームオブジェクトを生成
	GameObject* gameObject = scene->CreateGameObject();
	//位置を設定
	gameObject->GetTransform().translate = spawnPos;
	//名前を設定
	gameObject->SetName("落雷");

	//Object3dを追加
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	object3d->SetModel("lightningStrike");

	//落雷を追加
	gameObject->AddComponent<LightningStrike>();
}
