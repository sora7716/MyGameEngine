#pragma once
#include "GameObject.h"
#include "PhysicsData.h"
#include "PrimitiveData.h"
#include <cstdint>
#include <functional>

//レイヤー
enum class Layer : uint32_t {
	kNone = 0,
	kPlayer = 1 << 0,
	kEnemy = 1 << 1,
	kWall = 1 << 2,
	kGround = 1 << 3,
	kItem = 1 << 4
};

//Colliderの状態
//scalePtr: スケールのポインタ
//rotatePtr: 回転のポインタ
//translatePtr: 平行移動のポインタ
//velocityPtr: 速度のポインタ
//isOnGroundPtr: 地面にいるかのフラグのポインタ
//tag: オブジェクトのタグ
struct ColliderState {
	Vector3* scalePtr;
	Quaternion* rotatePtr;
	Vector3* translatePtr;
	Vector3* velocityPtr;
	bool* isOnGroundPtr;
	Tag *tagPtr;

	/// <summary>
    /// 初期化
    /// </summary>
    /// <param name="gameObject">ゲームオブジェクト</param>
    /// <param name="physicsData">物理演算データ</param>
	/// <param name="scale">スケール</param>
	void Initialize(GameObject& gameObject, PhysicsData& physicsData,Vector3& scale);
};

//動かせるのか動かせないのか
enum class BodyType {
	kStatic,
	kDynamic
};

//Collider
//owner: どの物体の当たり判定
//obb: 当たり判定の形
//isTrigger: 押し戻ししない
//isEnabled: 無効化用
//layer: 自分の所属レイヤー
//maskLayer: 当たりたい相手(複数)
//bodyType: 動かせるか動かせないのか
//onCollision: 衝突したときに呼ばれる
struct Collider {
	ColliderState* owner;
	PrimitiveData::OBB obb;
	bool isTrigger;
	bool isEnabled;
	Layer layer;
	uint32_t maskLayer;
	BodyType bodyType;
	std::function<void(ColliderState* other)>onCollision;
};
