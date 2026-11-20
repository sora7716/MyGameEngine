#pragma once
#include "Vector3.h"
#include <string>
#include <vector>
#include <memory>

//前方宣言
class BaseScene;
class GameObject;
class BoxCollider;
class Object3d;
class MapChipField;
class FollowCameraController;
class BaseDefenseObject;
class PlacementPreviewComponent;
class DefenseRotationComponent;
class PlacementSwitchComponent;
class DefensePlaceComponent;

//方向
enum class DirectionType :uint32_t {
	kLeft,
	kRight,
	kCount
};

//ディフェンスモデルタイプ
enum class DefenseModelType :uint32_t {
	kWall,
	kThunder,
	kFreeze,
	kBow,
	kGameMine
};

/// <summary>
/// 防衛オブジェクトの設置システム
/// </summary>
class DefensePlacementSystem {
public://静的メンバ変数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="scene">シーン</param>
	/// <param name="mapChipField">マップチップフィールド</param>
	/// <param name="playerObject">プレイヤーのゲームオブジェクト</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<DefensePlacementSystem>Create(BaseScene* scene,MapChipField* mapChipField,GameObject* playerObject);
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DefensePlacementSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DefensePlacementSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scene">シーン</param>
	/// <param name="mapChipField">マップチップフィールド</param>
	/// <param name="playerObject">プレイヤーのゲームオブジェクト</param>
	void Initialize(BaseScene* scene,MapChipField* mapChipField,GameObject* playerObject);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 設置予定を表示/非表示の取得
	/// </summary>
	/// <returns>設置予定を表示/非表示</returns>
	PlacementPreviewComponent* GetPlacementPreview()const;

	/// <summary>
	/// 設置するオブジェクトの切り替えの取得
	/// </summary>
	/// <returns>設置するオブジェクトの切り替え</returns>
	PlacementSwitchComponent* GetPlacementSwitch()const;

	/// <summary>
	/// 設置オブジェクトの回転の取得
	/// </summary>
	/// <returns>設置オブジェクトの回転</returns>
	DefenseRotationComponent* GetDefenseRotation()const;

	/// <summary>
	/// オブジェクトの設置する機能の取得
	/// </summary>
	/// <returns>オブジェクトの設置する機能</returns>
	DefensePlaceComponent* GetDefensePlace()const;
private://メンバ関数
	/// <summary>
	/// オブジェクトの設置予定地の生成
	/// </summary>
	/// <returns>オブジェクト</returns>
	void CreatePlacementPreview();

	/// <summary>
	/// オブジェクトのおける位置のインデックスを取得
	/// </summary>
	/// <param name="position">位置</param>
	/// <returns>成功したか</returns>
	bool TryGetPlacementIndex(const Vector3& position,Vector3Int& index);

	/// <summary>
	/// ディフェンス用のオブジェクトを生成
	/// </summary>
	void CreateDefenseObject();

	/// <summary>
	/// オブジェクトの設置予定地の更新
	/// </summary>
	void UpdatePlacementPreviewObject();

	/// <summary>
	/// ディフェンス用のオブジェクトのタイプによってコライダーの形や位置を切り替え
	/// </summary>
	/// <param name="modelName">モデル名</param>
	/// <param name="boxCollider">Boxコライダー</param>
	void SwitchDefenseObjectCollider(const std::string& modelName,BoxCollider* boxCollider);
private://メンバ変数
	//シーン
	BaseScene* scene_ = nullptr;

	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	//プレイヤーのゲームオブジェクト
	GameObject* playerObject_ = nullptr;

	//守るオブジェクトのモデル名の一覧
	std::vector<std::string>defenseObjectModelNameList_;

	//設置予定地のオブジェクト
	GameObject* placementPreviewObject_ = nullptr;
	Object3d* placementPreviewObject3d_ = nullptr;
	BoxCollider* placementPreviewBoxCollider_ = nullptr;
	Vector3Int placementPreviewIndex_ = {};

	//設置できるか
	bool isPlacementEnabled_ = false;

	//設置予定を表示/非表示
	PlacementPreviewComponent* placementPreviewComponent_ = nullptr;
	//設置するオブジェクトの切り替え
	PlacementSwitchComponent* placementSwitchComponent_ = nullptr;
	//設置オブジェクトの回転
	DefenseRotationComponent* defenseRotationComponent_ = nullptr;
	//オブジェクトの設置する機能
	DefensePlaceComponent* defensePlaceComponent_ = nullptr;
	//基盤となるディフェンス用のオブジェクト
	BaseDefenseObject* defenseObject_ = nullptr;
};

