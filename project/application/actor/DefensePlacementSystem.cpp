#include "DefensePlacementSystem.h"
#include "BaseScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Model.h"
#include "BoxCollider.h"
#include "BaseDefenseObject.h"
#include "PlacementPreviewComponent.h"
#include "DefenseRotationComponent.h"
#include "PlacementSwitchComponent.h"
#include "DefensePlaceComponent.h"
#include "MapChipField.h"
#include "MathUtility.h"
#include "Color.h"
#include <cassert>
using namespace mapChipDesc;

//生成
std::unique_ptr<DefensePlacementSystem> DefensePlacementSystem::Create(BaseScene* scene,MapChipField* mapChipField,GameObject* playerObject) {
	std::unique_ptr<DefensePlacementSystem>instance = std::make_unique<DefensePlacementSystem>();
	instance->Initialize(scene,mapChipField,playerObject);
	return instance;
}

//コンストラクタ
DefensePlacementSystem::DefensePlacementSystem() {
}

//デストラクタ
DefensePlacementSystem::~DefensePlacementSystem() {
}

//初期化
void DefensePlacementSystem::Initialize(BaseScene* scene,MapChipField* mapChipField,GameObject* playerObject) {
	//シーンの記録
	assert(scene);
	scene_ = scene;
	//マップチップフィールドの記録
	assert(mapChipField);
	mapChipField_ = mapChipField;
	//プレイヤーのゲームオブジェクトを設定
	assert(playerObject);
	playerObject_ = playerObject;

	//モデル一覧を登録
	defenseObjectModelNameList_.push_back("wall");
	defenseObjectModelNameList_.push_back("thunderTower");
	defenseObjectModelNameList_.push_back("freezeTower");
	defenseObjectModelNameList_.push_back("bowTower");
	defenseObjectModelNameList_.push_back("gameMine");

	//オブジェクトの設置予定地の生成
	CreatePlacementPreview();
}

//更新
void DefensePlacementSystem::Update() {
	//モデルが切り替わったら回転をリセット
	if(placementSwitchComponent_->IsSwitched()) {
		defenseRotationComponent_->ResetRotation();
	}

	//ディフェンスオブジェクトの設置
	if(placementPreviewComponent_->GetEnabledRequest()) {
		CreateDefenseObject();
	}

	//オブジェクトの設置予定地の更新
	UpdatePlacementPreviewObject();
}

//設置予定を表示/非表示の取得
PlacementPreviewComponent* DefensePlacementSystem::GetPlacementPreview() const {
	return placementPreviewComponent_;
}

//設置するオブジェクトの切り替えの取得
PlacementSwitchComponent* DefensePlacementSystem::GetPlacementSwitch() const {
	return placementSwitchComponent_;
}

//設置オブジェクトの回転の取得
DefenseRotationComponent* DefensePlacementSystem::GetDefenseRotation() const {
	return defenseRotationComponent_;
}

//オブジェクトの設置する機能の取得
DefensePlaceComponent* DefensePlacementSystem::GetDefensePlace() const {
	return defensePlaceComponent_;
}

//オブジェクトの設置予定地の生成
void DefensePlacementSystem::CreatePlacementPreview() {
	//ゲームオブジェクトを生成
	placementPreviewObject_ = scene_->CreateGameObject();
	GameObject* gameObject = placementPreviewObject_;
	Transform& transform = gameObject->GetTransform();
	transform.translate.z = 1.0f;
	gameObject->SetName("設置予定地");

	//Object3dの設定
	placementPreviewObject3d_ = gameObject->AddComponent<Object3d>();

	//設置予定地の表示/非表示用の機能を追加
	placementPreviewComponent_ = gameObject->AddComponent<PlacementPreviewComponent>();

	//設置物の回転用機能を追加
	defenseRotationComponent_ = gameObject->AddComponent<DefenseRotationComponent>();

	//設置物の切り替え用機能を追加
	placementSwitchComponent_ = gameObject->AddComponent<PlacementSwitchComponent>();
	//設置物の切り替え用機能のセットアップ
	placementSwitchComponent_->Setup(defenseObjectModelNameList_);

	//オブジェクトの設置する機能を追加
	defensePlaceComponent_ = gameObject->AddComponent<DefensePlaceComponent>();

	//BaseDefenderObjectを追加
	defenseObject_ = gameObject->AddComponent<BaseDefenseObject>();

	//衝突判定を設定
	placementPreviewBoxCollider_ = gameObject->AddComponent<BoxCollider>();
	placementPreviewBoxCollider_->SetIsTrigger(true);
}

//オブジェクトのおける位置のインデックスを取得
bool DefensePlacementSystem::TryGetPlacementIndex(const Vector3& position,Vector3Int& index) {
	//今いるインデックスを取得
	Vector3Int currentIndex = mapChipField_->GetMapChipIndexByPosition(position);
	//足元のインデックスを取得
	Vector3Int underIndex = {currentIndex.x,currentIndex.y - 1,currentIndex.z};
	//足元のステータスを取得
	//検索キーに値を代入
	index = currentIndex;

	//足元にブロックがなければ
	if(!mapChipField_->CheckOnCollisionMapChipBlock(underIndex)) {
		return false;
	}

	//現在のステータスを取得
	if(mapChipField_->CheckOnCollisionMapChipBlock(currentIndex)) {
		return false;
	}

	//現在のモデルが壁だった場合
	std::string modelName = placementSwitchComponent_->GetCurrentModelName();
	if(modelName == defenseObjectModelNameList_[static_cast<uint32_t>(DefenseModelType::kWall)]) {
		//モデルの向きを取得
		DefenseOrientation orientation = defenseRotationComponent_->GetDefenseOrientation();
		//横回転の場合
		if(orientation == DefenseOrientation::kHorizontal) {
			//右側にブロックがあるか
			Vector3Int rightIndex = {currentIndex.x + 1,currentIndex.y,currentIndex.z};
			if(mapChipField_->CheckOnCollisionMapChipBlock(rightIndex)) {
				return false;
			}

			//左側にブロックがあるか
			Vector3Int leftIndex = {currentIndex.x - 1,currentIndex.y,currentIndex.z};
			if(mapChipField_->CheckOnCollisionMapChipBlock(leftIndex)) {
				return false;
			}

			//右下側にブロックがあるか
			Vector3Int rightUnderIndex = {currentIndex.x + 1,currentIndex.y - 1,currentIndex.z};
			if(!mapChipField_->CheckOnCollisionMapChipBlock(rightUnderIndex)) {
				return false;
			}

			//左下側にブロックがあるか
			Vector3Int leftUnderIndex = {currentIndex.x - 1,currentIndex.y - 1,currentIndex.z};
			if(!mapChipField_->CheckOnCollisionMapChipBlock(leftUnderIndex)) {
				return false;
			}
		}

		//縦回転の場合
		if(orientation == DefenseOrientation::kVertical) {
			//前側にブロックがあるか
			Vector3Int frontIndex = {currentIndex.x,currentIndex.y,currentIndex.z + 1};
			if(mapChipField_->CheckOnCollisionMapChipBlock(frontIndex)) {
				return false;
			}

			//後側にブロックがあるか
			Vector3Int backIndex = {currentIndex.x,currentIndex.y,currentIndex.z - 1};
			if(mapChipField_->CheckOnCollisionMapChipBlock(backIndex)) {
				return false;
			}

			//前下側にブロックがあるか
			Vector3Int frontUnderIndex = {currentIndex.x,currentIndex.y - 1,currentIndex.z + 1};
			if(!mapChipField_->CheckOnCollisionMapChipBlock(frontUnderIndex)) {
				return false;
			}

			//後下側にブロックがあるか
			Vector3Int backUnderIndex = {currentIndex.x,currentIndex.y - 1,currentIndex.z - 1};
			if(!mapChipField_->CheckOnCollisionMapChipBlock(backUnderIndex)) {
				return false;
			}
		}
	}

	return true;
}

//ディフェンス用のオブジェクトの生成
void DefensePlacementSystem::CreateDefenseObject() {
	//設置の要求がない場合
	if(!defensePlaceComponent_->GetConfirmRequest()) {
		return;
	}

	//設置要求をリセット
	defensePlaceComponent_->SetConfirmRequest(false);

	//設置できない場合
	if(!isPlacementEnabled_ || defenseObject_->IsOverlappingDefense()) {
		return;
	}

	//ゲームオブジェクトの生成
	GameObject* gameObject = scene_->CreateGameObject();
	gameObject->SetName("ディフェンスオブジェクト");
	//タグを設定
	gameObject->SetTag("DefenseObject");
	//Transformをコピー
	gameObject->GetTransform() = placementPreviewObject_->GetTransform();

	//Object3dを追加
	Object3d* object3d = gameObject->AddComponent<Object3d>();
	std::string modelName = placementSwitchComponent_->GetCurrentModelName();
	object3d->SetModel(modelName);
	//LODの設定
	object3d->SetLODDistances({5.0f,8.0f,15.0f});

	//衝突判定を設定
	BoxCollider* boxCollider = gameObject->AddComponent<BoxCollider>();
	//モデル名によってコライダー形や位置を変更
	SwitchDefenseObjectCollider(modelName,boxCollider);
}

//オブジェクトの設置予定地の更新
void DefensePlacementSystem::UpdatePlacementPreviewObject() {
	//設置オブジェクトの回転が行えるかを設定
	defenseRotationComponent_->SetIsRunning(placementPreviewComponent_->GetEnabledRequest());
	//オブジェクトの切り替えが行えるかを設定
	placementSwitchComponent_->SetIsRunning(placementPreviewComponent_->GetEnabledRequest());

	//そもそも表示されて無ければ
	if(!placementPreviewComponent_->GetEnabledRequest()) {
		//衝突判定
		placementPreviewBoxCollider_->SetEnabled(false);
		return;
	}

	//衝突判定
	placementPreviewBoxCollider_->SetEnabled(true);

	//モデル名
	std::string modelName = placementSwitchComponent_->GetCurrentModelName();
	//モデル名によってコライダー形や位置を変更
	SwitchDefenseObjectCollider(modelName,placementPreviewBoxCollider_);

	//ブロックのおける位置を探す
	Matrix4x4 worldMatrix = Matrix4x4::Identity4x4();
	if(playerObject_) {
		worldMatrix = playerObject_->GetWorldMatrix();
	}
	Vector3 searchPos = Vector3::GetForward() * worldMatrix;
	isPlacementEnabled_ = TryGetPlacementIndex(searchPos,placementPreviewIndex_);

	//モデルを設定
	placementPreviewComponent_->SetModel(placementSwitchComponent_->GetCurrentModelName());

	//ブロックのおけるワールド座標の取得
	Vector3 placementPreviewPos = mapChipField_->GetMapChipPositionByIndex(placementPreviewIndex_);

	//マテリアルの色を設定
	Color materialColor = {1.0f,0.0f,0.0f,0.5f};

	//おける位置にいる場合
	if(isPlacementEnabled_ && !defenseObject_->IsOverlappingDefense()) {
		//おける位置にいた場合マテリアルを変更
		materialColor = {0.0f,1.0f,0.0f,0.5f};
	}

	//各メッシュにカラーを設定
	for(uint32_t i = 0; i < placementPreviewObject3d_->GetMaterialInstances().size(); i++) {
		placementPreviewObject3d_->SetMaterialParameter(i,"color",materialColor);
		placementPreviewObject3d_->SetMaterialParameter(i,"enabledLighting",uint32_t(0));
	}

	//座標を設定
	placementPreviewComponent_->SetPosition(placementPreviewPos);
}

//ディフェンス用のオブジェクトのタイプによってコライダーの形や位置を切り替え
void DefensePlacementSystem::SwitchDefenseObjectCollider(const std::string& modelName,BoxCollider* boxCollider) {
	//サイズのリセット
	boxCollider->SetHalfSize(Vector3::GetOne() / 2.0f);
	boxCollider->SetOffset(Vector3::GetZero());

	//モデル名によってコライダー形や位置を変更
	if(modelName == defenseObjectModelNameList_[static_cast<uint32_t>(DefenseModelType::kWall)]) {
		boxCollider->SetHalfSize({1.45f,0.5f,0.45f});
		boxCollider->SetOffset({0.0f,0.0f,0.0f});
	} else if(modelName != defenseObjectModelNameList_[static_cast<uint32_t>(DefenseModelType::kGameMine)]) {
		boxCollider->SetHalfSize({0.35f,1.0f,0.35f});
		boxCollider->SetOffset({0.0f,0.5f,0.0f});
	} else {
		boxCollider->SetIsTrigger(true);
		boxCollider->SetHalfSize({0.45f,0.15f,0.45f});
		boxCollider->SetOffset({0.0f,-0.4f,0.0f});
	}
}