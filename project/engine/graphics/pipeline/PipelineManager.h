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
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<PipelineManager>Create(ConstructorKey key, DirectXBase* directXBase);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit PipelineManager(ConstructorKey);

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
	//コピーコンストラクタ禁止
	PipelineManager(const PipelineManager&) = delete;
	//代入演算子の禁止
	PipelineManager& operator=(const PipelineManager&) = delete;

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

