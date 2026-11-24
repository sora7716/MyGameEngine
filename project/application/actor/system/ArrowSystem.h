#pragma once
#include "Matrix4x4.h"
#include <vector>
#include <memory>

//前方宣言
class BaseScene;
class GameObject;

/// <summary>
/// 矢の管理システム
/// </summary>
class ArrowSystem {
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="scene">シーン</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ArrowSystem>Create(BaseScene* scene);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ArrowSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ArrowSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scene">シーン</param>
	void Initialize(BaseScene* scene);

	/// <summary>
	/// 撃つ
	/// </summary>
	/// <param name="attackTarget">攻撃対象</param>
	/// <param name="headWorldMatrix">頭のワールド行列</param>
	void Fire(GameObject* attackTarget,const Matrix4x4& headWorldMatrix);
private://メンバ関数
	/// <summary>
	/// 矢の生成
	/// </summary>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateArrow();
private://定数
	//矢の最大数
	static inline const uint32_t kArrowMaxCount = 256;
private://メンバ変数
	//シーン
	BaseScene* scene_ = nullptr;
	//矢のゲームオブジェクト
	std::vector<GameObject*>arrowObjects_;
};

