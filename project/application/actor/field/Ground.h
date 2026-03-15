#pragma once
#include "ActorData.h"

//前方宣言
class Object3dCommon;
class Camera;

/// <summary>
/// 地面
/// </summary>
class Ground {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Ground();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Ground();

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
	//エンティティの塊
	EntityGroup entityGroup_ = {};
};

