#pragma once
#include "PipelineManagerData.h"
#include "BlendMode.h"
#include "RasterizerMode.h"
#include <string>
#include <memory>
#include <unordered_map>

//前方宣言
class DirectXBase;
class GraphicsPipeline;
class Blend;

//パイプラインの検索キー
struct PipelineKey{
	std::string pixelShaderFilePath = "";
	PipelineType pipelineType = PipelineType::kObject3d;
	BlendMode blendMode = BlendMode::kNone;
	rasterizerMode::CullingMode cullingMode = rasterizerMode::CullingMode::kNone;
	rasterizerMode::FillMode fillingMode = rasterizerMode::FillMode::kSolid;

	//パイプライン同じか確認
	bool operator==(const PipelineKey& pipelineKey)const;

	/// <summary>
	/// ハッシュキーを作成
	/// </summary>
	/// <returns>ハッシュキー</returns>
	size_t MakeHashKey()const;
};

//パイプラインの検索キーのハッシュを作成
struct PipelineKeyHasher{
	/// <summary>
	/// パイプラインの検索キーからハッシュを作成する関数を取得
	/// </summary>
	/// <param name="key">検索キー</param>
	/// <returns>ハッシュ</returns>
	size_t operator()(const PipelineKey& key)const;
};
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
	/// PSOの取得(存在しない場合は確保してキャッシュする)
	/// </summary>
	/// <param name="key">検索キー</param>
	const PipelineSet& GetPSO(PipelineKey key);
private://メンバ関数
	//コピーコンストラクタ禁止
	PipelineManager(const PipelineManager&) = delete;
	//代入演算子の禁止
	PipelineManager& operator=(const PipelineManager&) = delete;

	/// <summary>
	/// PSOの作成(Object3d)
	/// </summary>
	/// <param name="key">検索キー</param>
	void CreatePSOForObject3d(const PipelineKey& key);

	/// <summary>
	/// PSOの作成(Sprite)
	/// </summary>
	/// <param name="key">検索キー</param>
	void CreatePSOForSprite(const PipelineKey& key);

	/// <summary>
	/// PSOの作成(Particle)
	/// </summary>
	/// <param name="key">検索キー</param>
	void CreatePSOForParticle(const PipelineKey& key);

	/// <summary>
	/// PSOの作成(SkyBox)
	/// </summary>
	/// <param name="key">検索キー</param>
	void CreatePSOForSkyBox(const PipelineKey& key);

	/// <summary>
	/// PSOの作成(Wireframe)
	/// </summary>
	/// <param name="key">検索キー</param>
	void CreatePSOForWireframe(const PipelineKey& key);
private://静的メンバ変数
	//PSOの作成関数のテーブル
	static void (PipelineManager::* createPSOTable[])(const PipelineKey& key);
private://メンバ変数
	//ブレンド
	std::unique_ptr<Blend> blend_ = nullptr;
	//グラフィックスパイプライン
	std::unique_ptr<GraphicsPipeline> graphicsPipeline_ = nullptr;
	//各オブジェクトごとにパイプラインタイプ
	std::unordered_map<PipelineKey, PipelineSet, PipelineKeyHasher>pipelineSets_ = {};
};

