#include "DefensePlacementSystem.h"
#include "BaseScene.h"
#include "GameObject.h"
#include "Object3d.h"
#include "BoxCollider.h"
#include "BaseDefenseObject.h"
#include "PlacementPreviewComponent.h"
#include "DefenseRotationComponent.h"
#include "PlacementSwitchComponent.h"
#include "DefensePlaceComponent.h"
#include "MapChipField.h"
#include "Color.h"
#include "Wall.h"
#include "BowTower.h"
#include "FreezeTower.h"
#include "ThunderTower.h"
#include "GameMine.h"
#include "Indicator.h"
#include "SphereCollider.h"
#include "Health.h"
#include "HealthBar.h"
#include <cassert>
using namespace mapChipDesc;

//生成
std::unique_ptr<DefensePlacementSystem> DefensePlacementSystem::Create(BaseScene* scene,MapChipField* mapChipField,GameObject* playerObject,ArrowSystem* arrowSystem) {
	std::unique_ptr<DefensePlacementSystem>instance = std::make_unique<DefensePlacementSystem>();
	instance->Initialize(scene,mapChipField,playerObject,arrowSystem);
	return instance;
}

//コンストラクタ
DefensePlacementSystem::DefensePlacementSystem() {
}

//デストラクタ
DefensePlacementSystem::~DefensePlacementSystem() {
}

//初期化
void DefensePlacementSystem::Initialize(BaseScene* scene,MapChipField* mapChipField,GameObject* playerObject,ArrowSystem* arrowSystem) {
	//シーンの記録
	assert(scene);
	scene_ = scene;
	//マップチップフィールドの記録
	assert(mapChipField);
	mapChipField_ = mapChipField;
	//プレイヤーのゲームオブジェクトを記録
	assert(playerObject);
	playerObject_ = playerObject;
	//矢の管理システムを記録
	assert(arrowSystem);
	arrowSystem_ = arrowSystem;

	//モデル一覧を登録
	defenseObjectModelNameList_.push_back("wall");
	defenseObjectModelNameList_.push_back("thunderTower");
	defenseObjectModelNameList_.push_back("freezeTower");
	defenseObjectModelNameList_.push_back("bowTower");
	defenseObjectModelNameList_.push_back("gameMine");

	//オブジェクトの設置予定地の生成
	CreatePlacementPreview();

	//テクスチャファイルパス
	MaterialTexturePaths textureFilePaths = {
		.textureFilePath = "textures/white1x1.png",
		.environmentMap = "textures/skybox_cube.dds"
	};

	//弓のインジケータのマテリアルを作成
	scene->AddMaterialInstance("BowIndicatorMaterial",textureFilePaths);
	MaterialInstance* bowIndicatorMat = scene->FindMaterialInstance("BowIndicatorMaterial");
	bowIndicatorMat->SetParameter("color",Color::ColorCodeTransform("#F5AD5580"));
	bowIndicatorMat->SetParameter("enabledLighting",uint32_t{0});
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

//敵の設定
void DefensePlacementSystem::SetEnemies(const std::vector<GameObject*>& enemyObjects) {
	enemyObjects_ = enemyObjects;
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
	//マテリアルを生成
	scene_->AddMaterialInstance("previewMat");

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

	//インジケータを作成
	placementPreviewIndicatorObject_ = scene_->CreateGameObject();
	placementPreviewIndicatorObject_->AttachTo(gameObject,false);
	//Object3dの追加
	placementPreviewIndicatorObject3d_ = placementPreviewIndicatorObject_->AddComponent<Object3d>();
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

	//HPの管理を追加
	Health* health = gameObject->AddComponent<Health>();

	//モデルごとにHPを設定
	SetHPByModelName(modelName,health);

	//モデルによってコンポーネントを変更する
	AddComponentsByModelName(modelName,gameObject);

	//モデルごとにインジケーターを生成する
	CreateRangeIndicator(modelName,gameObject);

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
		//インジケータも非表示
		placementPreviewIndicatorObject3d_->SetEnabled(false);
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


	//マテリアルインスタンスを取得
	MaterialInstance* materialInstance = scene_->FindMaterialInstance("previewMat");
	//マテリアルの色を設定
	Color materialColor = {1.0f,0.0f,0.0f,0.5f};

	//おける位置にいる場合
	if(isPlacementEnabled_ && !defenseObject_->IsOverlappingDefense()) {
		//おける位置にいた場合マテリアルを変更
		materialColor = {0.0f,1.0f,0.0f,0.5f};
	}
	//色をマテリアルインスタンスに設定
	materialInstance->SetParameter("color",materialColor);
	//ライティングを切る
	materialInstance->SetParameter("enabledLighting",uint32_t{0});

	//各メッシュにカラーを設定
	for(uint32_t i = 0; i < placementPreviewObject3d_->GetMaterialInstances().size(); i++) {
		placementPreviewObject3d_->SetMaterialInstance(i,scene_->GetMaterialInstance("previewMat"));
	}

	//インジケータの生成
	SwitchPlacementIndicator(modelName,scene_->GetMaterialInstance("previewMat"));

	//座標を設定
	placementPreviewComponent_->SetPosition(placementPreviewPos);
}

//ディフェンス用のオブジェクトのタイプによってコライダーの形や位置を切り替え
void DefensePlacementSystem::SwitchDefenseObjectCollider(const std::string& modelName,BoxCollider* boxCollider) {
	//サイズのリセット
	boxCollider->SetSize(Vector3::GetOne() / 2.0f);
	boxCollider->SetOffset(Vector3::GetZero());

	//モデル名によってコライダー形や位置を変更
	if(modelName == GetModelNameByModelType(DefenseModelType::kWall)) {
		boxCollider->SetSize({2.9f,1.0f,0.95f});
		boxCollider->SetOffset({0.0f,0.0f,0.0f});
	} else if(modelName != GetModelNameByModelType(DefenseModelType::kGameMine)) {
		boxCollider->SetSize({0.95f,2.0f,0.95f});
		boxCollider->SetOffset({0.0f,0.5f,0.0f});
	} else {
		boxCollider->SetIsTrigger(true);
		boxCollider->SetSize({0.95f,0.3f,0.95f});
		boxCollider->SetOffset({0.0f,-0.4f,0.0f});
	}
}

//モデル名によって追加するコンポーネントを変える
void DefensePlacementSystem::AddComponentsByModelName(const std::string& modelName,GameObject* gameObject) {
	//モデルが壁なら
	if(modelName == GetModelNameByModelType(DefenseModelType::kWall)) {
		gameObject->AddComponent<Wall>();
	}
	//モデルが弓タワーなら
	if(modelName == GetModelNameByModelType(DefenseModelType::kBow)) {
		BowTower* bowTower = gameObject->AddComponent<BowTower>();
		bowTower->Setup(arrowSystem_,bowScanRadius_);
		bowTower->SetTargets(enemyObjects_);
	}
	//モデルが氷結タワーなら
	if(modelName == GetModelNameByModelType(DefenseModelType::kFreeze)) {
		gameObject->AddComponent<FreezeTower>();
	}
	//モデルが雷タワーなら
	if(modelName == GetModelNameByModelType(DefenseModelType::kThunder)) {
		gameObject->AddComponent<ThunderTower>();
	}
	//モデルが地雷なら
	if(modelName == GetModelNameByModelType(DefenseModelType::kGameMine)) {
		gameObject->AddComponent<GameMine>();
	}
}

//ディフェンスオブジェクトごとにインジケーターの生成
void DefensePlacementSystem::CreateRangeIndicator(const std::string& modelName,GameObject* defenseObject) {
	//壁なら
	if(modelName == GetModelNameByModelType(DefenseModelType::kWall)) {
		return;
	}
	//インジケータのオブジェクトを追加
	GameObject* sensor = scene_->CreateGameObject();
	sensor->SetName("探索範囲");
	//親子付け
	sensor->AttachTo(defenseObject,false);
	//球のコライダーを追加
	SphereCollider* sphereCollider = sensor->AddComponent<SphereCollider>();
	sphereCollider->SetBodyType(BodyType::kDynamic);
	sphereCollider->SetIsTrigger(true);
	sphereCollider->SetRadius(3.0f);
	//弓モデルの場合
	if(modelName == GetModelNameByModelType(DefenseModelType::kBow)) {
		//扇形の表示だけを拡縮する
		GameObject* visual = scene_->CreateGameObject();
		visual->SetName("visual");
		visual->AttachTo(sensor,false);

		//インジケーターのTransform
		Transform& indicatorTransform = visual->GetTransform();
		indicatorTransform.translate.y = -0.45f;
		indicatorTransform.SetEulerAngleDegrees({0.0f,180.0f,0.0f});

		//モデルを設定
		Object3d* object3d = visual->AddComponent<Object3d>();
		object3d->SetModel("scanRange");
		object3d->SetMaterialInstance(0,scene_->GetMaterialInstance("BowIndicatorMaterial"));

		//弓のタワーを取得
		BowTower* bowTower = defenseObject->GetComponent<BowTower>();
		assert(bowTower);
		float showTargetRadius = bowTower->GetScanRadius();

		//インジケータの設定
		Indicator* indicator = sensor->AddComponent<Indicator>();
		//見た目だけのオブジェクトを設定
		indicator->SetVisualObject(visual);
		//表示する半径を設定
		indicator->SetShowTargetRadius(showTargetRadius);
	}
}

//設置予定のモデルによって表示するインジケータの切り替え
void DefensePlacementSystem::SwitchPlacementIndicator(const std::string& modelName,const std::shared_ptr<MaterialInstance>& materialInstance) {
	//インジケータを非表示
	placementPreviewIndicatorObject3d_->SetEnabled(false);
	//壁なら
	if(modelName == GetModelNameByModelType(DefenseModelType::kWall)) {
		return;
	}

	//インジケーターのTransform
	Transform& indicatorTransform = placementPreviewIndicatorObject_->GetTransform();
	//弓のタワーなら
	if(modelName == GetModelNameByModelType(DefenseModelType::kBow)) {
		//インジケータを表示
		placementPreviewIndicatorObject3d_->SetEnabled(true);

		//位置を設定
		indicatorTransform.translate.y = -0.45f;
		//角度を設定
		indicatorTransform.SetEulerAngleDegrees({0.0f,180.0f,0.0f});
		//サイズを設定
		indicatorTransform.scale = {bowScanRadius_,bowScanRadius_,bowScanRadius_};

		//モデルとマテリアルの設定
		placementPreviewIndicatorObject3d_->SetModel("scanRange");
		placementPreviewIndicatorObject3d_->SetMaterialInstance(0,materialInstance);
	}
}

//モデル名をモデルタイプから取得
std::string DefensePlacementSystem::GetModelNameByModelType(DefenseModelType modelType) {
	return defenseObjectModelNameList_[static_cast<uint32_t>(modelType)];
}


//モデル名ごとにHPを設定
void DefensePlacementSystem::SetHPByModelName(const std::string& modelName,Health* health) {
	//壁なら
	if(modelName == GetModelNameByModelType(DefenseModelType::kWall)) {
		health->SetMaxHp(wallInitialHP_);
		//HPバーの生成
		CreateHealthBar(health);
	}
}

//HPバーの生成
void DefensePlacementSystem::CreateHealthBar(Health* health) {
	//接続されているGameObjectを取得
	GameObject* gameObject = health->GetOwner();

	//HPバーの子オブジェクトを作成
	GameObject* healthObject = scene_->CreateGameObject();
	healthObject->SetName("HPバー");
	//親子付け
	healthObject->AttachTo(gameObject,false);

	//Object3dの追加
	Object3d* healthBarObject3d = healthObject->AddComponent<Object3d>();
	healthBarObject3d->SetModel("healthBar");
	healthBarObject3d->SetRenderTransformMode(RenderTransformMode::kBillboard);
	healthBarObject3d->SetMaterialInstance(0,scene_->GetMaterialInstance("enemyHpBarMat"));
	//HealthBarを追加
	HealthBar* healthBar = healthObject->AddComponent<HealthBar>();
	healthBar->Setup(health);

	//HPが満タンのときの半分の横幅を取得
	constexpr float kBarMeshWidth = 0.5f;
	const float halfFullWidth = kBarMeshWidth * healthBar->GetFullBarScale() / 2.0f;
	healthBarObject3d->SetBillboardHorizontalOffset(-halfFullWidth);

	//HealthBarの後ろ背景
	GameObject* healthBarBgObject = scene_->CreateGameObject();
	//Transformの調整
	Transform& bgTransform = healthBarBgObject->GetTransform();
	const Transform& barTransform = healthObject->GetTransform();
	bgTransform.translate = barTransform.translate;
	bgTransform.scale.x = healthBar->GetFullBarScale();
	bgTransform.scale.y = barTransform.scale.y;
	healthBarBgObject->SetName("HPバーの背景");

	//親子付け
	healthBarBgObject->AttachTo(gameObject,false);

	//Object3dの追加
	Object3d* healthBarBgObject3d = healthBarBgObject->AddComponent<Object3d>();
	healthBarBgObject3d->SetModel("healthBar");
	healthBarBgObject3d->SetRenderTransformMode(RenderTransformMode::kBillboard);
	healthBarBgObject3d->SetMaterialInstance(0,scene_->GetMaterialInstance("hpBarBgMat"));
	healthBarBgObject3d->SetBillboardDepthOffset(0.01f);
	healthBarBgObject3d->SetBillboardHorizontalOffset(-halfFullWidth);
}
