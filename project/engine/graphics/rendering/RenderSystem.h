#pragma once
//前方宣言
class DirectXBase;
class PipelineManager;
class Blend;

/// <summary>
/// 描画のシステム
/// </summary>
class RenderSystem{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RenderSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~RenderSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤</param>
	/// <param name="pipelineManager">パイプラインの管理</param>
	void Initialize(DirectXBase* directXBase, PipelineManager* pipelineManager);

	/// <summary>
	/// 描画の開始
	/// </summary>
	void PreDraw();
private://メンバ関数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//パイプラインの管理
	PipelineManager* pipelineManager_ = nullptr;
};

