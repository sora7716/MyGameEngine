#pragma once
#include "ActorData.h"

//前方宣言
class Object3dCommon;
class Camera;

/// <summary>
/// 地面の基底クラス
/// </summary>
class BaseGround{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseGround();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseGround();

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
protected://メンバ変数
	//エンティティの塊
	EntityGroup entityGroup_ = {};
};

