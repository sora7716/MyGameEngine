#pragma once
#include "Vector3.h"
#include "EnemyData.h"
#include <vector>
#include <memory>

//前方宣言
class BaseScene;
class GameObject;

/// <summary>
/// 落雷管理のシステム
/// </summary>
class LightningStrikeSystem {
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="scene">シーン</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<LightningStrikeSystem>Create(BaseScene* scene);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LightningStrikeSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LightningStrikeSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scene">シーン</param>
	void Initialize(BaseScene* scene);

	/// <summary>
	/// 落雷する
	/// </summary>
	/// <param name="targetPos">落ちる場所</param>
	void Strike(const Vector3& targetPos);

	/// <summary>
	/// ショックパラメータを設定
	/// </summary>
	/// <param name="parameter">ショックパラメータ</param>
	void SetShockParameter(const ShockParameter& parameter);
private://メンバ関数
	/// <summary>
	/// 落雷の生成
	/// </summary>
	/// <returns></returns>
	GameObject* CreateLightningStrike();
private://定数
	//落雷の最大数
	static inline const uint32_t kLightningStrikeMaxCount = 1024;
private://メンバ変数
	//シーン
	BaseScene* scene_ = nullptr;
	//落雷の配列
	std::vector<GameObject*>lightningStrikeObjects_;
	//ショックパラメータ
	ShockParameter parameter_ = {};
};

