#define NOMINMAX
#include "MapChipCollider.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "ImGuiManager.h"
#include "MathUtility.h"
#include <algorithm>
#include <cassert>
using namespace mapChipDesc;
//コンストラクタ
MapChipCollider::MapChipCollider(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
MapChipCollider::~MapChipCollider() {
}

//セットアップ
void MapChipCollider::Setup(MapChipField* mapChipField,RigidBody* rigidBody,const Vector3& targetSize) {
	//マップチップフィールドの記録
	assert(mapChipField);
	mapChipField_ = mapChipField;
	//リジッドボディの記録
	assert(rigidBody);
	rigidBody_ = rigidBody;
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
	//衝突判定を行う
	CollisionMapChipInfo collisionInfo = {};
	collisionInfo.moveAmount = rigidBody_->GetMoveAmount(mathUtility::kDeltaTime);
	//衝突判定を確認
	CheckCollision(collisionInfo);

	//衝突したときの反応
	ApplyCollisionResponse(collisionInfo);
}

//デバッグでImGuiを使えるようにする
void MapChipCollider::DebugImGui() {
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

//更新のフェーズ
UpdatePhase MapChipCollider::GetUpdatePhase() {
	return UpdatePhase::kLate;
}

//衝突判定の確認
void MapChipCollider::CheckCollision(CollisionMapChipInfo& info) {
	//今の位置を取得
	const Vector3 worldPos = GetOwner()->GetWorldPos();

	//Y軸:XとZの移動してない状態で判定
	Vector3 center = worldPos;
	center.x -= info.moveAmount.x;
	center.z -= info.moveAmount.z;
	CheckCollisionBottom(info,center);

	//X軸:Zの移動だけしてない状態で判定
	center.x += info.moveAmount.x;
	//右面の衝突判定
	CheckCollisionRight(info,center);
	//左面の衝突判定
	CheckCollisionLeft(info,center);

	//Z軸:すべての位置を戻して判定
	center.z += info.moveAmount.z;
	//正面の衝突判定
	CheckCollisionFront(info,center);
	////背面の衝突判定
	CheckCollisionBack(info,center);

	//補正した分をまとめては寧
	Transform& transform = GetOwner()->GetTransform();
	transform.translate += center - worldPos;//ワールド分は引いてローカルへ戻す
}

//正面の衝突判定を確認
void MapChipCollider::CheckCollisionFront(CollisionMapChipInfo& info,Vector3& center) {
	//前に移動してない場合
	if(info.moveAmount.z <= 0.0f) {
		return;
	}

	//動く前の前面
	float prevFront = center.z + targetHalfSize_.z - info.moveAmount.z;
	//確認したい角を取得
	std::array<Corner,4>corners = {
		Corner::kRightUpFront,
		Corner::kLeftUpFront,
		Corner::kRightBottomFront,
		Corner::kLeftBottomFront
	};

	//衝突している範囲矩形を取得
	std::vector<Rect>rects = GetCollisionRects(center,corners);

	//衝突しているか
	bool isHit = false;
	//限界
	float limitZ = 0.0f;
	//微小な値
	constexpr float epsilon = 0.001f;
	//範囲矩形にアクセス
	for(const Rect& rect : rects) {
		//動く前からブロックより後ろにいたら
		if(prevFront > rect.back + epsilon) {
			continue;
		}

		//移動させる位置
		float z = rect.back - targetHalfSize_.z - kBlank;
		//衝突していないなら
		if(!isHit) {
			limitZ = z;
		}

		//小さい値を選択
		if(z < limitZ) {
			limitZ = z;
		}

		//衝突している
		isHit = true;
	}

	//衝突していないなら
	if(!isHit) {
		return;
	}

	//衝突しているなら
	//押し戻す
	center.z += limitZ - center.z;
	//ブロックに当たった
	info.isCollisionZ = true;
}

//背面の衝突判定を確認
void MapChipCollider::CheckCollisionBack(CollisionMapChipInfo& info,Vector3& center) {
	//後ろに移動してない場合
	if(info.moveAmount.z >= 0.0f) {
		return;
	}

	//動く前の背面
	float prevBack = center.z - targetHalfSize_.z - info.moveAmount.z;
	//確認したい角を取得
	std::array<Corner,4>corners = {
		Corner::kRightUpBack,
		Corner::kLeftUpBack,
		Corner::kRightBottomBack,
		Corner::kLeftBottomBack
	};

	//衝突している範囲矩形を取得
	std::vector<Rect>rects = GetCollisionRects(center,corners);

	//衝突しているか
	bool isHit = false;
	//限界
	float limitZ = 0.0f;
	//微小な値
	constexpr float epsilon = 0.001f;
	//範囲矩形にアクセス
	for(const Rect& rect : rects) {
		//動く前からブロックより手前にいたら
		if(prevBack < rect.front + epsilon) {
			continue;
		}

		//移動させる位置
		float z = rect.front + targetHalfSize_.z + kBlank;
		//衝突していないなら
		if(!isHit) {
			limitZ = z;
		}

		//大きい値を選択
		if(z > limitZ) {
			limitZ = z;
		}

		//衝突している
		isHit = true;
	}

	//衝突していないなら
	if(!isHit) {
		return;
	}

	//衝突しているなら
	//押し戻す
	center.z += limitZ - center.z;
	//ブロックに当たった
	info.isCollisionZ = true;
}

//右面の衝突判定
void MapChipCollider::CheckCollisionRight(CollisionMapChipInfo& info,Vector3& center) {
	//右に移動してない場合
	if(info.moveAmount.x <= 0.0f) {
		return;
	}

	//動く前の右面
	float prevRight = center.x + targetHalfSize_.x - info.moveAmount.x;
	//確認したい角を取得
	std::array<Corner,4>corners = {
		Corner::kRightUpBack,
		Corner::kRightBottomBack,
		Corner::kRightBottomFront,
		Corner::kRightUpFront
	};

	//衝突している範囲矩形を取得
	std::vector<Rect>rects = GetCollisionRects(center,corners);

	//衝突しているか
	bool isHit = false;
	//限界
	float limitX = 0.0f;
	//微小な値
	constexpr float epsilon = 0.001f;
	//範囲矩形にアクセス
	for(const Rect& rect : rects) {
		//動く前からブロックより右にいたら
		if(prevRight > rect.left + epsilon) {
			continue;
		}

		//移動させる位置
		float x = rect.left - targetHalfSize_.x - kBlank;
		//衝突していないなら
		if(!isHit) {
			limitX = x;
		}

		//小さい値を選択
		if(x < limitX) {
			limitX = x;
		}

		//衝突している
		isHit = true;
	}

	//衝突していないなら
	if(!isHit) {
		return;
	}

	//衝突しているなら
	//押し戻す
	center.x += limitX - center.x;
	//ブロックに当たった
	info.isCollisionX = true;
}

//左面の衝突判定を確認
void MapChipCollider::CheckCollisionLeft(CollisionMapChipInfo& info,Vector3& center) {
	//左に移動しない場合
	if(info.moveAmount.x >= 0.0f) {
		return;
	}

	//動く前の左面
	float prevLeft = center.x - targetHalfSize_.x - info.moveAmount.x;
	//確認したい角を取得
	std::array<Corner,4>corners = {
		Corner::kLeftUpBack,
		Corner::kLeftBottomBack,
		Corner::kLeftBottomFront,
		Corner::kLeftUpFront
	};

	//衝突している範囲矩形を取得
	std::vector<Rect>rects = GetCollisionRects(center,corners);

	//衝突しているか
	bool isHit = false;
	//限界
	float limitX = 0.0f;
	//微小な値
	constexpr float epsilon = 0.001f;
	//範囲矩形にアクセス
	for(const Rect& rect : rects) {
		//動く前からブロックより左にいたら
		if(prevLeft < rect.right + epsilon) {
			continue;
		}

		//移動させる位置
		float x = rect.right + targetHalfSize_.x + kBlank;
		//衝突していないなら
		if(!isHit) {
			limitX = x;
		}

		//大きい値を選択
		if(x > limitX) {
			limitX = x;
		}

		//衝突している
		isHit = true;
	}

	//衝突していないなら
	if(!isHit) {
		return;
	}

	//衝突しているなら
	//押し戻す
	center.x += limitX - center.x;
	//ブロックに当たった
	info.isCollisionX = true;
}

//下面からの衝突判定を確認
void MapChipCollider::CheckCollisionBottom(CollisionMapChipInfo& info,Vector3& center) {
	//落下してない場合
	if(info.moveAmount.y >= 0.0f) {
		return;
	}

	//動く前の下面
	float prevBottom = center.y - targetHalfSize_.y - info.moveAmount.y;
	//確認したい角を取得
	std::array<Corner,4>corners = {
		Corner::kRightBottomBack,
		Corner::kLeftBottomBack,
		Corner::kRightBottomFront,
		Corner::kLeftBottomFront
	};

	//衝突している範囲矩形を取得
	std::vector<Rect>rects = GetCollisionRects(center,corners);

	//衝突しているか
	bool isHit = false;
	//限界
	float limitY = 0.0f;
	//微小な値
	constexpr float epsilon = 0.001f;
	//範囲矩形にアクセス
	for(const Rect& rect : rects) {
		//動く前からブロックの上面より下にいた
		if(prevBottom < rect.top - epsilon) {
			continue;
		}

		//移動させる位置
		float y = rect.top + targetHalfSize_.y;
		//衝突していないなら
		if(!isHit) {
			limitY = y;
		}

		//小さい値を選択
		if(y < limitY) {
			limitY = y;
		}

		//衝突している
		isHit = true;
	}

	//衝突していないなら
	if(!isHit) {
		return;
	}

	//衝突しているなら
	//押し戻す
	center.y = limitY;
	//ブロックに当たった
	info.isOnGround = true;
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
		}
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

//衝突したときの反応
void MapChipCollider::ApplyCollisionResponse(const CollisionMapChipInfo& info) {
	//速度を取得
	Vector3& velocity = rigidBody_->GetVelocity();

	//前後に壁に当たった
	if(info.isCollisionZ) {
		velocity.z = 0.0f;
	}
	//左右の壁に当たった
	if(info.isCollisionX) {
		velocity.x = 0.0f;
	}
	//着地した
	if(info.isOnGround) {
		velocity.y = 0.0f;
	}

	//設置状態をリジッドボディに送信
	rigidBody_->SetExternalGround(info.isOnGround);
}

//衝突した範囲矩形の配列を取得
std::vector<Rect> MapChipCollider::GetCollisionRects(const Vector3& center,const std::array<Corner,4>& corners) {
	//範囲矩形
	std::vector<Rect>rects;
	//各頂点にアクセス
	for(Corner corner : corners) {
		//角の現在の位置
		Vector3 cornerPositionNow = GetCornerPosition(center,corner);
		//真正面の当たり判定を行う
		Vector3Int index = mapChipField_->GetMapChipIndexByPosition(cornerPositionNow);
		MapChipType type = mapChipField_->GetMapChipTypeByIndex(index);
		//ブロックがなければ次へ
		if(type != MapChipType::kBlock) {
			continue;
		}

		//当たったブロックの矩形記録
		rects.push_back(mapChipField_->GetRectByIndex(index));
	}

	return rects;
}
