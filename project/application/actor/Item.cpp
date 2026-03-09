#include "Item.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "WireframeObject3d.h"
#include "ImGuiManager.h"
#include "Score.h"

//コンストラクタ
Item::Item() {
}

//デストラクタ
Item::~Item() {
}

//初期化
void Item::Initialize(Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName) {
	//オブジェクトの数
	entityGroup_.objectCount = 1;
	//インスタンス数
	//instance_.reserve(100);
	//エンティティの配列の大きさを決める
	entityGroup_.entity.resize(entityGroup_.objectCount);
	//モデル名
	entityGroup_.modelName = modelName;
	//レンダーオブジェクトの初期化
	entityGroup_.renderObject = entityGroup_.renderObject
		.Create()
		.InitializeMaterial()
		.Build();
	//3dオブジェクトの初期化
	entityGroup_.renderObject.object3d->Initialize(object3dCommon, camera, entityGroup_.objectCount);
	entityGroup_.renderObject.object3d->SetModel(entityGroup_.modelName);

	//ヒットボックス
	entityGroup_.renderObject.hitBox->Initialize(object3dCommon->GetWireframeObject3dCommon(), camera, ModelType::kCube, entityGroup_.objectCount);

	//エンティティ初期化
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//ゲームオブジェクトの初期化
		entityGroup_.entity[i].gameObject.Initialize();
		entityGroup_.entity[i].gameObject.transformData.translate.z = -18.0f;
		entityGroup_.entity[i].gameObject.isAlive = true;
		//ヒットボックスのスケール
		entityGroup_.entity[i].hitBoxScale = Vector3::MakeAllOne();
		//コライダーの状態の初期化
		entityGroup_.entity[i].colliderState.Initialize(entityGroup_.entity[i].hitBoxScale, entityGroup_.entity[i].gameObject, Tag::kItem);

		//コライダーの初期化
		entityGroup_.entity[i].collider = entityGroup_.entity[i].collider
			.SetOwner(&entityGroup_.entity[i].colliderState)
			.SetIsTrigger(false)
			.SetIsEnebled(false)
			.SetBodyType(BodyType::kDynamic)
			.SetLayer(Layer::kItem)
			.SetMaskLayer(ToBits(Layer::kWall) | ToBits(Layer::kGround) | ToBits(Layer::kPlayer))
			.SetOnCollision([this, i](ColliderState* other) {this->OnCollision(i, other); })
			.Build();
	}
}

//更新
void Item::Update() {
	int32_t aliveCount = 0;

	//生成の処理
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		//敵の生成
		if (!entityGroup_.entity[i].gameObject.isAlive) {
			continue;
		}

		//衝突判定をオン
		entityGroup_.entity[i].collider.SetIsEnebled(true);

		// 生存だけを 0..aliveCount-1 に詰める
		entityGroup_.renderObject.object3d->SetTransformData(
			aliveCount,
			entityGroup_.entity[i].gameObject.transformData
		);

		// ヒットボックスも同じ index にする（デバッグ表示目的なら）
		entityGroup_.renderObject.hitBox->SetTranslate(aliveCount, entityGroup_.entity[i].gameObject.transformData.translate);
		entityGroup_.renderObject.hitBox->SetQuaternion(aliveCount, entityGroup_.entity[i].gameObject.transformData.quaternion);
		entityGroup_.renderObject.hitBox->SetScale(aliveCount, entityGroup_.entity[i].hitBoxScale);

		++aliveCount;
	}
	// 描画に使う数を保存（メンバにして Draw で使う）
	aliveCount_ = aliveCount;

	entityGroup_.renderObject.object3d->Update();
	entityGroup_.renderObject.hitBox->Update();
}

//デバッグ
void Item::Debug() {
#ifdef _DEBUG
	for (int32_t i = 0; i < entityGroup_.objectCount; i++) {
		ImGui::PushID(i);
		ImGui::SeparatorText(("item " + std::to_string(i)).c_str());
		ImGui::DragFloat3("hitBox.scale", &entityGroup_.entity[i].hitBoxScale.x, 0.1f);
		ImGuiManager::DragTransform(entityGroup_.entity[i].gameObject.transformData);
		ImGui::Checkbox("isAlive", &entityGroup_.entity[i].gameObject.isAlive);
		ImGui::Checkbox("isEnableCollision", &entityGroup_.entity[i].collider.isEnabled);
		ImGui::PopID();
	}
#endif // _DEBUG
}

//衝突したら
void Item::OnCollision(int32_t index, ColliderState* other) {
	if (other->tag == Tag::kPlayer) {
		//衝突判定をオフ
		entityGroup_.entity[index].collider.SetIsEnebled(false);
		//生存フラグをオフ
		entityGroup_.entity[index].gameObject.isAlive = false;
		//スコアを加算
		Score::AddScore(100);
	}
}

//描画
void Item::Draw() {//敵の描画
	entityGroup_.renderObject.object3d->Draw(aliveCount_);

	//ヒットボックスの描画
	entityGroup_.renderObject.hitBox->Draw(aliveCount_);
}

//カメラのセッター
void Item::SetCamera(Camera* camera) {
	entityGroup_.renderObject.object3d->SetCamera(camera);
}

//エンティティのゲッター
std::vector<Entity>& Item::GetEntity() {
	return entityGroup_.entity;
}
