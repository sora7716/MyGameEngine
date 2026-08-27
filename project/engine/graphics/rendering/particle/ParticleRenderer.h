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
private://構造体
	//パーティクルの情報をGPUに送るための構造体
	struct ParticleForGPU{
		Matrix4x4 world = Matrix4x4::Identity4x4();
		Vector4 color = Vector4::MakeWhiteColor();
	};

	//パーティクルのGPUリソース
	struct GpuResource{
		Microsoft::WRL::ComPtr<ID3D12Resource>instancingResource = nullptr;
		ParticleForGPU* instanceData = nullptr;
		uint32_t srvIndex = 0;
		uint32_t capacity = 0;

		//マテリアル
		std::vector<Material*> materialData;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>materialResources;
	};
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

	/// <summary>
	/// パーティクルを登録
	/// </summary>
	/// <param name="maxInstance">インスタンスの最大値</param>
	/// <returns>パーティクルのハンドル</returns>
	ParticleRenderHandle RegisterParticle(uint32_t maxInstance);
private://メンバ関数
	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	/// <param name="gpuResource">パーティクルのGpuに送るデータ</param>
	void CreateTransformationMatrixResource(GpuResource& gpuResource);

	/// <summary>
	/// インスタンシングリソースのストラクチャバッファの生成
	/// </summary>
	/// <param name="gpuResource">パーティクルのGpuに送るデータ</param>
	void CreateStructuredBufferForParticleGpu(GpuResource& gpuResource);

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	/// <param name="gpuResource">パーティクルのGpuに送るデータ</param>
	/// <param name="materialCount">マテリアル数</param>
	void CreateMaterialResources(GpuResource& gpuResource,uint32_t materialCount);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
	//Textureの管理
	TextureManager* textureManager_ = nullptr;
	//描画データ
	std::vector<ParticleRenderData>renderDatas_;
	//GPUリソース
	std::vector<GpuResource>gpuResources_;
};

