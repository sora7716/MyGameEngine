#pragma once
#include "ParticleSystem.h"
#include <map>
#include <memory>
#include <string>

/// <summary>
/// パーティクルの管理クラス
/// </summary>
class ParticleManager {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleManager();

	/// <summary>
	/// パーティクルシステムの追加
	/// </summary>
	/// <param name="name">名前</param>
	void AddParticleSystem(const std::string& name);

	/// <summary>
	/// パーティクルシステムを検索
	/// </summary>
	/// <param name="name">名前</param>
	ParticleSystem* FindParticleSystem(const std::string& name);
public://PrassKey
	class ConstructorKey {
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PrassKeyを受け取る</param>
	explicit ParticleManager(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタ禁止
	ParticleManager(const ParticleManager&) = delete;
	//代入演算子を禁止
	ParticleManager operator=(const ParticleManager&) = delete;
public://静的メンバ変数
	//インスタンス
	static inline ParticleManager* instance = nullptr;
	//終了処理をしたかのフラグ
	static inline bool isFinalize = false;
private://メンバ変数
	//パーティクルシステムのマップ
	std::map<std::string, std::unique_ptr<ParticleSystem>> particleSystems_;
};

