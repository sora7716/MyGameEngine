#pragma once
#include <string>
#include <vector>
#include "ActorData.h"
#include "RenderingData.h"
#include "EnemyState.h"
#include "PrimitiveData.h"

//前方宣言
class Object3dCommon;
class Object3d;
class Camera;
class WireframeObject3d;
class Bullet;

/// <summary>
/// 敵
/// </summary>
class Enemy {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">3Dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	/// <param name="modelName">モデル名</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera, const std::string& modelName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 衝突したら
	/// </summary>
	/// <param name="index">何番目が当たったのか</param>
	/// <param name="other">誰と当たったのか</param>
	void OnCollision(int32_t index, ColliderState* other);

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// ターゲットの位置のセッター
	/// </summary>
	/// <param name="targetPos">ターゲットの位置</param>
	void SetTarget(const Vector3& targetPos);

	/// <summary>
	/// エンティティのゲッター
	/// </summary>
	/// <returns>エンティティ</returns>
	std::vector<Entity>& GetEntity();
public://敵の行動
	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();
private://メンバ関数
	/// <summary>
	/// ターゲットの方向を向く
	/// </summary>
	void EnemyToTarget();

	/// <summary>
	/// 敵の振る舞い
	/// </summary>
	void Behavior();

	/// <summary>
	/// 速度と加速度を位置に適応
	/// </summary>
	void IntegrateMotion();

	/// <summary>
	/// ステートの切り替え
	/// </summary>
	/// <param name="next">次のステート</param>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <param name="collider">コライダーのセッター</param>
	void ChangeState(IEnemyState* next, GameObject& gameObject, Collider& collider);
private://定数
	//待機時の回転速度
	static inline const float kIdolRotSpeed = 0.5f;
	//攻撃タイマーのリミット
	static inline const float kAttackTimerLimit = 5.0f;
	//生存エリアのサイズ
	static inline const float kAliveAreaSize = 100.0f;
	//HPの最大値
	static inline const int32_t kMaxHpCout = 5;
private://メンバ変数
	//敵の最初のスポーンテーブル
	std::vector<Vector3>enemySpawnTable_;

	//エンティティ
	EntityGroup entityGroup_ = {};
	int32_t aliveCount_ = 0;

	//ターゲットの位置
	Vector3 targetPos_ = {};

	//敵の状態
	std::unique_ptr <IEnemyState> spawn_ = nullptr;
	std::unique_ptr <IEnemyState> idol_ = nullptr;
	std::unique_ptr <IEnemyState> chase_ = nullptr;
	IEnemyState* currentState_ = nullptr;

	float spawnTimer_ = 0.0f;

	//攻撃フラグ
	bool isAttacking_ = false;
	//攻撃タイマー
	float attackTimer_ = 0.0f;

	//ワイヤーフレーム
	//球
	std::unique_ptr <WireframeObject3d> sphere_ = nullptr;
	float sphereRadius_ = 0.0f;
	//行動が変化するエリア
	std::unique_ptr <WireframeObject3d> attackArea = nullptr;
	float attackAreaRadius_ = 0.0f;
};

