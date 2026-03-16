#pragma once
#include "ActorData.h"
#include <windows.h>

//前方宣言
class Object3dCommon;
class Camera;
class Input;

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
	/// <param name="object3dCommon">3dオブジェクトの共通部分</param>
	/// <param name="camera">カメラ</param>
	/// <param name="input">入力</param>
	void Initialize(Object3dCommon* object3dCommon, Camera* camera, Input* input);

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
private://メンバ関数
	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// ジャンプ
	/// </summary>
	void Jump();
private://メンバ変数
	//エンティティグループ
	EntityGroup entityGroup_ = {};
	//移動方向
	Vector3 moveDirection_ = {};
	//入力
	Input* input_ = nullptr;
	//Xboxのナンバー
	DWORD xboxNumber_ = 0;
	//カメラ
	Camera* camera_ = nullptr;
};

