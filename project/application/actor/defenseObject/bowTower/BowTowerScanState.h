#pragma once
#include "IState.h"
#include "Vector3.h"

//前方宣言
class BowTower;

/// <summary>
/// 弓タワーの探索ステート
/// </summary>
class BowTowerScanState :public IState {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BowTowerScanState();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BowTowerScanState()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="gameObject"></param>
	void Setup(GameObject* gameObject)override;

	/// <summary>
	/// 入っ瞬間
	/// </summary>
	void Enter()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 終了
	/// </summary>
	void Exit()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns>複製</returns>
	std::unique_ptr<IState>Clone()override;
private://メンバ関数
	/// <summary>
	/// 探しているときのアニメーションの初期化
	/// </summary>
	void InitializeScanAnimation();

	/// <summary>
	/// 探しているときのアニメーションの更新
	/// </summary>
	void UpdateScanAnimation();
private://メンバ変数
	//弓のタワー
	BowTower* bowTower_ = nullptr;
	//探索をする最大角度(度)
	float maxScanRotate_ = 75.0f;
	//探索をする最小角度(度)
	float minScanRotate_ = -75.0f;
	//探索する際の回転オフセット
	float scanRotOffset_ = {};
	//探索する速度
	float scanSpeed_ = 2.0f;
	//今探索している向き
	float scanDir_ = 1.0f;
	//スキャン開始位置
	Vector3 scanBeginRotate = {};
};

