#define NOMINMAX
#include "MapChipCollider.h"
#include "MapChipField.h"
#include "GameObject.h"
#include <algorithm>
#include <cassert>
#include <array>
//コンストラクタ
MapChipCollider::MapChipCollider(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
MapChipCollider::~MapChipCollider() {
}

//セットアップ
void MapChipCollider::Setup(MapChipField* mapChipField,const Vector3& targetSize) {
	assert(mapChipField);
	mapChipField_ = mapChipField;
	//マップチップのデータを取得
	mapChipData_ = mapChipField_->GetMapChipData();
	//対象のサイズ
	targetSize_ = targetSize;
	//対象のハーフサイズ
	targetHalfSize_ = targetSize_ / 2.0f;
}

//初期化
void MapChipCollider::Initialize() {
	//ゲームオブジェクトの取得
	gameObject_ = GetOwner();
}

//更新
void MapChipCollider::Update() {

}

//複製
std::unique_ptr<Component> MapChipCollider::Clone(GameObject* gameObject) const {
	std::unique_ptr<MapChipCollider>cloneInstance = std::make_unique<MapChipCollider>(gameObject);

	//初期化
	cloneInstance->Initialize();

	//Cameraが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());

	return cloneInstance;
}

//コンポーネントのタイプ名を取得
std::string MapChipCollider::GetTypeName() const {
	return "MapChipCollider";
}

//衝突判定の確認
void MapChipCollider::CheckCollision(const CollisionMapChipInfo& info) {
	//正面の衝突判定
	CheckCollision(info);
}

//正面の衝突判定を確認
void MapChipCollider::CheckCollisionFront(CollisionMapChipInfo& info) {
	//正面に移動したか
	if(info.moveAmount.z > 0.0f) {
		return;
	}

	//中心座標
	Vector3 center = GetOwner()->GetWorldPos();

	//各角の計算
	std::array<Vector3,static_cast<uint32_t>(Corner::kNumConer)>positionNew = {};
	for(uint32_t i = 0; i < positionNew.size(); i++) {
		positionNew[i] = GetCornerPosition(center + info.moveAmount,static_cast<Corner>(i));
	}

	//真正面の当たり判定を行う
	MapChipType type = MapChipType::kBlank;
	bool hit = false;
	Vector3Int index = {};

	//右上前の判定
	index = mapChipField_->GetMapChipIndexByPosition(positionNew[static_cast<uint32_t>(Corner::kRightUpFront)]);
	type = mapChipField_->GetMapChipType(index);
	if(type != MapChipType::kBlank) {
		hit = true;
	}

	//左上前の判定
	index = mapChipField_->GetMapChipIndexByPosition(positionNew[static_cast<uint32_t>(Corner::kLeftUpFront)]);
	type = mapChipField_->GetMapChipType(index);
	if(type != MapChipType::kBlank) {
		hit = true;
	}

	//右下前の判定
	index = mapChipField_->GetMapChipIndexByPosition(positionNew[static_cast<uint32_t>(Corner::kRightBottomFront)]);
	type = mapChipField_->GetMapChipType(index);
	if(type != MapChipType::kBlank) {
		hit = true;
	}

	//左下前の判定
	index = mapChipField_->GetMapChipIndexByPosition(positionNew[static_cast<uint32_t>(Corner::kLeftBottomFront)]);
	type = mapChipField_->GetMapChipType(index);
	if(type != MapChipType::kBlank) {
		hit = true;
	}

	//衝突していたら
	if(hit) {
		//めり込みを排除する移動量を設定
		index = mapChipField_->GetMapChipIndexByPosition(center + Vector3(0.0f,0.0f,targetHalfSize_.z) + info.moveAmount);
		//現在の座標がオブジェクトの外か判定
		Vector3Int indexNow = mapChipField_->GetMapChipIndexByPosition((center + Vector3(0.0f,0.0f,targetHalfSize_.z)));
		if(indexNow.z != index.z) {
			//めり込み先ブロックの矩形範囲
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(index);
			//移動量を計算
			info.moveAmount.z = std::min(0.0f,(rect.front - center.z) + (targetHalfSize_.z + kBlank));
			//ブロックに当たった
			info.isHitWall = true;
		}
	}

}

//各角のポジションを取得
Vector3 MapChipCollider::GetCornerPosition(const Vector3& center,Corner corner) {
	std::array<Vector3,static_cast<uint32_t>(Corner::kNumConer)> offsetTable{{
		{targetHalfSize_.x,-targetHalfSize_.y,-targetHalfSize_.z},//右下後
		{-targetHalfSize_.x,-targetHalfSize_.y,-targetHalfSize_.z},//左下後
		{targetHalfSize_.x,targetHalfSize_.y,-targetHalfSize_.z},//右上後
		{-targetHalfSize_.x,targetHalfSize_.y,-targetHalfSize_.z},//左上後
		{targetHalfSize_.x,-targetHalfSize_.y,targetHalfSize_.z},//右下前
		{-targetHalfSize_.x,-targetHalfSize_.y,targetHalfSize_.z},//左下前
		{targetHalfSize_.x,targetHalfSize_.y,targetHalfSize_.z},//右上前
		{-targetHalfSize_.x,targetHalfSize_.y,targetHalfSize_.z},//左上前
		}};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
