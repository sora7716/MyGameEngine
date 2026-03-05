#pragma once
#include "RenderingData.h"
#include "ResourceData.h"
#include "PrimitiveData.h"
#include "Quaternion.h"
#include <functional>
#include <memory>

//前方宣言
class Object3d;
class WireframeObject3d;

//タグ
enum class Tag {
	kPlayer,
	kEnemy,
	kWall,
	kGround,
	kGoal,
	kNone
};

//レイヤー
enum class Layer : uint32_t {
	kNone = 0,
	kPlayer = 1 << 0,
	kEnemy = 1 << 1,
	kWall = 1 << 2,
	kGround = 1 << 3
};

//ゲームオブジェクト
struct GameObject {
	TransformData transformData;
	Vector3 velocity;
	Vector3 acceleration;
	Vector3 direction;
	bool isAlive;
	bool isOnGround;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
};

//描画時に必要な物
//object3d: オブジェクト3d
//hitBox: ヒットボックス
//material: マテリアル
struct RenderObject {
	std::unique_ptr<Object3d> object3d;
	std::unique_ptr<WireframeObject3d> hitBox;
	Material material;

	/// <summary>
	/// 生成
	/// </summary>
	/// <returns>レンダーオブジェクト</returns>
	RenderObject& Create();

	/// <summary>
	/// マテリアルの初期化
	/// </summary>
	/// <returns>レンダーオブジェクト</returns>
	RenderObject& InitializeMaterial();

	/// <summary>
	/// 作成
	/// </summary>
	/// <returns>右辺値のレンダーオブジェクト</returns>
	RenderObject&& Build();
};

//弾
struct BulletData {
	GameObject gameObject;
	RenderObject renderObject;
	Vector3 direction;
	Vector3 shootingPoint;
	float aliveRange;
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
	Tag tag;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="hitBoxScale">ヒットボックスのスケール</param>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <param name="tag">タグ</param>
	void Initialize(Vector3& hitBoxScale, GameObject& gameObject, Tag tag = Tag::kNone);
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
	OBB obb;
	bool isTrigger;
	bool isEnabled;
	Layer layer;
	uint32_t maskLayer;
	BodyType bodyType;
	std::function<void(ColliderState* other)>onCollision;

	/// <summary>
	/// ownerのセッター
	/// </summary>
	/// <param name="colliderState">コライダーの状態</param>
	/// <returns>コライダー</returns>
	Collider& SetOwner(ColliderState* colliderState);

	/// <summary>
	/// OBBの生成
	/// </summary>
	/// <returns>コライダー</returns>
	Collider& CreateObb();

	/// <summary>
	/// BodyTypeのセッター
	/// </summary>
	/// <param name="bodyType">BodyType</param>
	/// <returns>コライダー</returns>
	Collider& SetBodyType(BodyType bodyType);

	/// <summary>
	/// isTriggerのセッター
	/// </summary>
	/// <param name="isTrigger">貫通させるか</param>
	/// <returns>コライダー</returns>
	Collider& SetIsTrigger(bool isTrigger);

	/// <summary>
	/// isEnableのセッター
	/// </summary>
	/// <param name="isEnabled">衝突判定を行うか</param>
	/// <returns>コライダー</returns>
	Collider& SetIsEnebled(bool isEnabled);


	/// <summary>
	/// レイヤーのセッター
	/// </summary>
	/// <param name="layer">レイヤー</param>
	/// <returns>コライダー</returns>
	Collider& SetLayer(Layer layer);

	/// <summary>
	/// マスクレイヤーのセッター
	/// </summary>
	/// <param name="maskLayer">マスクレイヤー</param>
	/// <returns>コライダー</returns>
	Collider& SetMaskLayer(uint32_t maskLayer);

	/// <summary>
	/// onCollisionのセッター
	/// </summary>
	/// <param name="onCollision">衝突したときの判定</param>
	/// <returns>コライダー</returns>
	Collider& SetOnCollision(std::function<void(ColliderState* other)>onCollision);

	/// <summary>
	/// 作成
	/// </summary>
	/// <returns>右辺値のコライダー</returns>
	Collider&& Build();
};

//実体
struct Entity {
	GameObject gameObject;
	Vector3 hitBoxScale;
	ColliderState colliderState;
	Collider collider;
};

//実体の塊
struct EntityGroup {
	std::vector<Entity>entity;
	RenderObject renderObject;
	std::string modelName;
	int32_t objectCount;
};

//Bitに変換
uint32_t ToBits(Layer layer);
