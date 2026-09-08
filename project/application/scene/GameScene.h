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
private://メンバ変数
	//AABBのコライダー
	AABBCollider* playerAABB_ = nullptr;
	//ヒットボックス用のキューブ
	debugDraw::Cube* playerCube_ = nullptr;
	//プレイヤーのヒットボックスのサイズ
	Vector3 playerHitBoxSize_ = {};
};