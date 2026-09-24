#pragma once
#include "RenderingData.h"

//前方宣言
class Player;
class Object3d;

/// <summary>
/// プレイヤーのステートのインターフェース
/// </summary>
class IPlayerState{
public://構造体
	//プレイヤーのTransform
	struct PlayerPose{
		Transform root = {};
		Transform head = {};
		Transform body = {};
		Transform leftArm = {};
		Transform rightArm = {};

		/// <summary>
		/// 補間
		/// </summary>
		/// <param name="playerPose1">プレイヤーポーズ</param>
		/// <param name="playerPose2">プレイヤーポーズ</param>
		/// <param name="t">係数</param>
		/// <returns>補間したプレイヤーポーズ</returns>
		static PlayerPose Lerp(const PlayerPose& playerPose1, const PlayerPose& playerPose2, float t);
	};
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IPlayerState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPlayerState();
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="player">プレイヤー</param>
	virtual void Initialize(Player* player) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// オブジェクト3dの設定
	/// </summary>
	/// <param name="object3d">オブジェクト3d</param>
	void SetObject3d(Object3d*object3d);
protected://メンバ関数
	/// <summary>
	/// 過去のポーズを設定
	/// </summary>
	void SettingPreviousPose();

	/// <summary>
	/// モーション遷移の初期化
	/// </summary>
	void InitializeTransition();

	/// <summary>
	/// モーション遷移の更新
	/// </summary>
	/// <param name="targetPose">目的のポーズ</param>
	void UpdateTransition(PlayerPose targetPose);
private://定数
	//モーションの切り替え時間
	static inline const float kTransitionDuration = 0.4f;
private://メンバ変数
	//モーション切り替え用のタイマー
	float transitionTimer_ = 0.0f;
protected://メンバ変数
	//プレイヤー
	Player* player_ = nullptr;
	//オブジェクト3d
	Object3d* object3d_ = nullptr;
	//今のNodeのLocalTransform情報
	PlayerPose currentPose_ = {};
	//前のNodeのLocalTransform情報
	PlayerPose prePose_ = {};
};

