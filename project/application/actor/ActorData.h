#pragma once
#include "RenderingData.h"
#include "ResourceData.h"
#include "PrimitiveData.h"
#include <functional>
#include <memory>

//前方宣言
class Object3d;
class WireframeObject3d;

//タグ
enum class Tag {
	kPlayer,
	kEnemy,
	kPlayerBullet,
	kEnemyBullet,
	kWall,
	kGround,
	kGoal,
	kNone
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
//worldMatrixPtr: ワールド行列のポインタ
//isOnGroundPtr: 地面にいるかのフラグのポインタ
//tag: オブジェクトのタグ
struct ColliderState {
	Vector3* scalePtr;
	Vector3* rotatePtr;
	Vector3* translatePtr;
	Vector3* velocityPtr;
	Matrix4x4* worldMatrixPtr;
	bool* isOnGroundPtr;
	Tag tag;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="hitBoxScale">ヒットボックスのスケール</param>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <param name="worldMatrix">ワールド行列</param>
	/// <param name="tag">タグ</param>
	void Initialize(Vector3& hitBoxScale,GameObject& gameObject, Matrix4x4& worldMatrix, Tag tag = Tag::kNone);
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
//bodyType: 動かせるか動かせないのか
//onCollision: 衝突したときに呼ばれる
struct Collider {
	ColliderState* owner;
	OBB obb;
	bool isTrigger;
	bool isEnabled;
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

	Collider& SetOnCollision(std::function<void(ColliderState* other)>onCollision);

	Collider&& Build();
};
