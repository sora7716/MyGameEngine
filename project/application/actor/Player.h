#pragma once
#include "ActorData.h"
#include <Windows.h>
#include <string>
#include <memory>
#include "PrimitiveData.h"
#include "PhysicsData.h"
#include "Vector4.h"

// 前方宣言
class Input;
class Camera;
class SpriteCommon;
class Object3dCommon;
class Sprite;
class WireframeObject3d;
class WorldTransform;
class Bullet;

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="input">入力</param>
	/// <param name="spriteCommon">スプライトの共通部分</param>
	/// <param name="object3dCommon">3Dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	/// <param name="modelName">モデル名</param>
	void Initialize(Input* input, SpriteCommon* spriteCommon, Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ用
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突したら
	/// </summary>
	/// <param name="other">ColliderState</param>
	void OnCollision(ColliderState*other);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 位置のセッター
	/// </summary>
	/// <param name="position">位置</param>
	void SetPosition(const Vector3& position);

	/// <summary>
	/// 速度のセッター
	/// </summary>
	/// <param name="velocity">速度</param>
	void SetVelocity(const Vector3& velocity);

	/// <summary>
	/// 地面の上にいるかのフラグのセッター
	/// </summary>
	/// <param name="isOnGround">地面の上にいるか</param>
	void SetIsOnGround(bool isOnGround);

	/// <summary>
	/// 地面の上にいるかのフラグのゲッター
	/// </summary>
	/// <returns>地面の上にいるか</returns>
	bool IsOnGround();

	/// <summary>
	/// ワールド座標系での位置のゲッター
	/// </summary>
	/// <returns>ワールド座標系での位置</returns>
	Vector3 GetWorldPos()const;

	/// <summary>
	/// トランスフォームデータのゲッター
	/// </summary>
	/// <returns>トランスフォームデータ</returns>
	TransformData GetTransformData()const;

	/// <summary>
	/// 速度のゲッター
	/// </summary>
	/// <returns>速度</returns>
	Vector3 GetVelocity()const;

	/// <summary>
	/// 弾のゲッター
	/// </summary>
	/// <returns>弾</returns>
	Bullet* GetBullet()const;

	/// <summary>
	/// 生存フラグのゲッター
	/// </summary>
	/// <returns>生存フラグ</returns>
	bool IsAlive();

	/// <summary>
	/// ゴールしたかのフラグのゲッター
	/// </summary>
	/// <returns>ゴールしたかのフラグ</returns>
	bool IsGoal();

	/// <summary>
	/// コライダーのゲッター
	/// </summary>
	/// <returns>コライダー</returns>
	Collider& GetCollider();
private://定数
	//カメラの移動速度
	static inline const float kMoveSpeed = 8.0f;
	//ジャンプするときの初速
	static inline const float kJumpSpeed = 15.0f;
	//HPの最大値
	static inline const int32_t kMaxHpCount = 10;
	//ダメージのクールタイムの最大値
	static inline const float kMaxDamageCoolTime = 0.5f;
	//弾の数
	static inline const uint32_t kBulletCount = 30;
	//弾の速度
	static inline const float kBulletSpeed = 30.0f;
	//弾の大きさ
	static inline const float kBulletSize = 0.25f;
	//弾の生存エリア
	static inline const float kBulletAliveAreaSize = 100.0f;
private://メンバ関数
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// ジャンプ
	/// </summary>
	void Jump();

	/// <summary>
	/// 速度と加速度を位置に適応
	/// </summary>
	void IntegrateMotion();

	/// <summary>
	/// ダメージを受ける
	/// </summary>
	void Damage();

	/// <summary>
	/// 死亡
	/// </summary>
	void Dead();

	/// <summary>
	/// ヘッドライトの更新
	/// </summary>
	void HeadlightUpdate();

	/// <summary>
	/// 視点移動
	/// </summary>
	void LookDirection();
private://メンバ変数
	//入力
	Input* input_ = nullptr;

	//3dオブジェクトの共通部分
	Object3dCommon* object3dCommon_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//ゲームオブジェクト
	GameObject gameObject_ = {};
	//描画で使用する
	RenderObject renderObject_ = {};

	//弾
	std::unique_ptr<Bullet> bullet_ = nullptr;

	//ヘッドライト
	SpotLight headlight_ = {};

	//リムライト
	RimLight rimLight_ = {};

	//Xboxの番号
	DWORD xBoxPadNumber_ = 0;

	//ヒットボックス
	Vector3 hitBoxScale_ = { 1.0f,1.0f,1.0f };

	//ヒットポイント
	int32_t hp_ = kMaxHpCount;
	float hpBarWidth_ = 400.0f;
	std::unique_ptr<Sprite> hpBar_ = nullptr;
	Vector4 hpColor_ = Vector4::MakeGreenColor();
	Transform2dData hpBarTransform_ = {};
	std::unique_ptr<Sprite> hpOutLine_ = nullptr;
	Transform2dData hpOutLineTransform_ = {};

	//ダメージのクールタイム
	float damageCoolTime_ = 0.0f;

	//衝突
	ColliderState colliderState_ = {};
	Collider collider_ = {};

	//ゴールしたら
	bool isGoal_ = false;
};

