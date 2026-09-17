#pragma once
#include "BaseScene.h"

//前方宣言
class AABBCollider;
namespace debugDraw{
	class Cube;
}

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :public BaseScene{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene()override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新のステート
	/// </summary>
	void UpdateState()override;

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug()override;

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize()override;
private://メンバ関数
	/// <summary>
	/// SkyBoxの生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateSkyBox();

	/// <summary>
	/// ゲームカメラの生成
	/// </summary>
	/// <param name="playerObject">プレイヤー</param>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateGameCamera(GameObject* playerObject);

	/// <summary>
	/// プレイヤーの生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreatePlayerObject();

	/// <summary>
	/// 地面の生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateGround();

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateEnemy();

	/// <summary>
	/// 剣の生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateSword();
private://メンバ変数
	//プレイヤー
	GameObject* playerObject_ = nullptr;

	//ゲームカメラ
	GameObject* gameCameraObject_ = nullptr;
};