#pragma once
#include "ParticleRenderData.h"
#include <cstdint>
#include <memory>

//前方宣言
class DirectXBase;
class SRVManager;
class TextureManager;

/// <summary>
/// パーティクルの描画
/// </summary>
class ParticleRenderer{
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">Textureの管理</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<ParticleRenderer>Create(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">Textureの管理</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	void Draw(uint32_t instanceIndex);

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const ParticleRenderData& renderData);

	/// <summary>
	/// 描画データのサイズを取得
	/// </summary>
	/// <returns>描画データのサイズを取得</returns>
	uint32_t GetRenderDataSize();

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <param name="instanceIndex">インスタンス検索キー</param>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode(uint32_t instanceIndex);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
	//Textureの管理
	TextureManager* textureManager_ = nullptr;
	//描画データ
	std::vector<ParticleRenderData>renderDatas_;
};

