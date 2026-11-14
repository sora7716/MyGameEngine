#pragma once
#include "Component.h"
#include "MapChipData.h"
#include "Vector3.h"

//前方宣言
class MapChipField;
class RigidBody;

//衝突判定の情報
struct CollisionMapChipInfo {
	bool isHitWall = false;
	bool isLanding = false;
	Vector3 moveAmount = {};
};

/// <summary>
/// マップチップのコライダー
/// </summary>
class MapChipCollider :public Component {
public://構造体
	//角
	enum class Corner :uint32_t {
		kRightBottomBack,//右下後
		kLeftBottomBack,//左下後
		kRightUpBack,//右上後
		kLeftUpBack,//左下後
		kRightBottomFront,//右下前
		kLeftBottomFront,//左下前
		kRightUpFront,//右上前
		kLeftUpFront,//左下前
		kNumConer//要素数
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit MapChipCollider(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MapChipCollider()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="mapChipField">マップチップのフィールド</param>
	/// <param name="rigidBody">リジッドボディ</param>
	/// <param name="targetSize">ターゲットのサイズ</param>
	void Setup(MapChipField* mapChipField,RigidBody* rigidBody,const Vector3& targetSize);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// デバッグでImGuiを使えるようにする
	/// </summary>
	void DebugImGui()override;

	/// <summary>
	/// コピー
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コピーしたインスタンス</returns>
	std::unique_ptr<Component> Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を取得
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;

	/// <summary>
	/// 更新のフェーズの取得
	/// </summary>
	/// <returns>更新のフェーズ</returns>
	UpdatePhase GetUpdatePhase();
private://メンバ関数
	/// <summary>
	/// 衝突判定の確認
	/// </summary>
	/// <param name="info">衝突情報</param>
	void CheckCollision(CollisionMapChipInfo& info);

	/// <summary>
	/// 正面の衝突判定を確認
	/// </summary>
	/// <param name="info">衝突情報</param>
	void CheckCollisionFront(CollisionMapChipInfo& info);

	/// <summary>
	/// 各角のポジションを取得
	/// </summary>
	/// <param name="center">中心</param>
	/// <param name="corner">角</param>
	/// <returns>位置</returns>
	Vector3 GetCornerPosition(const Vector3& center,Corner corner);

	/// <summary>
	/// 壁に衝突しているとき
	/// </summary>
	/// <param name="info">衝突情報</param>
	void WallCollision(const CollisionMapChipInfo& info);
private://定数
	//余白
	static inline const float kBlank = 0.01f;
	// 壁に衝突時の速度減衰率
	static inline const float kAttenuationWall = 0.45f;
private://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
	//マップのデータ
	MapChipData mapChipData_ = {};
	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;
	//対象のサイズ
	Vector3 targetSize_ = {};
	//ハーフサイズ
	Vector3 targetHalfSize_ = {};
	//リジッドボディ
	RigidBody* rigidBody_ = nullptr;

	bool isWall_ = false;
};

