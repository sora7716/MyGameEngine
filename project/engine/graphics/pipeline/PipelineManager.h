#pragma once
#include "PipelineManagerData.h"
#include <memory>

//前方宣言
class DirectXBase;
class GraphicsPipeline;
class Blend;

/// <summary>
/// パイプラインの管理
/// </summary>
class PipelineManager{	
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PipelineManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void Initialize(DirectXBase* directXBase);

	/// <summary>
	/// PSOの作成
	/// </summary>
	void CreatePSO();

	/// <summary>
	/// パイプラインセットの取得
	/// </summary>
	/// <param name="pipelineSetType">パイプラインセットのタイプ</param>
	/// <returns>パイプラインセット</returns>
	const PipelineSet& GetPipelineSet(PipelineType pipelineSetType)const;
private://メンバ関数
	/// <summary>
	/// PSOの作成(Object3d)
	/// </summary>
	void CreatePSOForObject3d();

	/// <summary>
	/// PSOの作成(Sprite)
	/// </summary>
	void CreatePSOForSprite();

	/// <summary>
	/// PSOの作成(Particle)
	/// </summary>
	void CreatePSOForParticle();

	/// <summary>
	/// PSOの作成(SkyBox)
	/// </summary>
	void CreatePSOForSkyBox();

	/// <summary>
	/// PSOの作成(DebugDraw)
	/// </summary>
	void CreatePSOForDebugDraw();
public://PassKeyIdiom
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit PipelineManager(ConstructorKey);
private://静的メンバ変数
	//PSOの作成関数のテーブル
	static void (PipelineManager::* createPSOTable[])();
private://メンバ変数
	//ブレンド
	std::unique_ptr<Blend> blend_ = nullptr;
	//グラフィックスパイプライン
	std::unique_ptr<GraphicsPipeline> graphicsPipeline_ = nullptr;
	//各オブジェクトごとにパイプラインタイプ
	std::array<PipelineSet, static_cast<uint32_t>(PipelineType::kPiplineTypeCount)> pipelineSets_ = {};
};

