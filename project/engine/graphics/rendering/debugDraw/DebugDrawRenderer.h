#pragma once
#include "DebugDrawRenderData.h"
#include <vector>
#include <memory>

//前方宣言
class DirectXBase;

/// <summary>
/// デバッグ描画のレンダラー
/// </summary>
class DebugDrawRenderer{
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<DebugDrawRenderer>Create(DirectXBase* directXBase);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DebugDrawRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugDrawRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase* directXBase);

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const DebugDrawRenderData& renderData);

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
	/// 描画データのサイズの取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetRenderDataSize();

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <param name="instanceIndex">インスタンスの検索キー</param>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode(uint32_t instanceIndex);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//デバッグ描画のレンダーデータ
	std::vector<DebugDrawRenderData>debugDrawRenderDatas_;
};

