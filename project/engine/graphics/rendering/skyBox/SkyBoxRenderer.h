#pragma once
#include "SkyBoxRendererData.h"
#include <vector>

//前方宣言
class DirectXBase;
class TextureManager;

/// <summary>
/// スカイボックスのレンダラー
/// </summary>
class SkyBoxRenderer{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SkyBoxRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SkyBoxRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="textureManager">テクスチャの管理</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager);

	/// <summary>
    /// 描画データの追加
    /// </summary>
	/// <param name="skyBoxRenderData"></param>
	void AddRenderData(const SkyBoxRenderData& skyBoxRenderData);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	void Draw(uint32_t instanceIndex);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;
	//スカイボックスの描画データ
	std::vector<SkyBoxRenderData> skyBoxRenderDatas_ = {};
};

