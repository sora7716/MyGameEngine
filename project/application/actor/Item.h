#pragma once
#include "ActorData.h"
#include <string>

//前方宣言
class Object3dCommon;
class Camera;

/// <summary>
/// アイテム
/// </summary>
class Item {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Item();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Item();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object3dCommon">Object3dの共通部分</param>
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
	/// 衝突したら
	/// </summary>
	/// <param name="index">何番目が当たったのか</param>
	/// <param name="other">衝突した物</param>
	void OnCollision(int32_t index, ColliderState* other);

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
	//生存カウント
	uint32_t aliveCount_ = 0;
};

