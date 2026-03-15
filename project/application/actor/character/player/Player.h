#pragma once
#include "ActorData.h"

//前方宣言
class Object3dCommon;
class Camera;

/// <summary>
/// プレイヤー
/// </summary>
class Player{
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
	/// <param name="object3dCommon">3dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera);

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
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// エンティティのゲッター
	/// </summary>
	/// <returns>エンティティ</returns>
	std::vector<Entity>& GetEntity();
private://メンバ変数
	//エンティティグループ
	EntityGroup entityGroup_ = {};
};

