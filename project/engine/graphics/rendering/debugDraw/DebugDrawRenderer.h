#pragma once
#include "DebugDrawRenderData.h"
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>

//前方宣言
class DirectXBase;
class Camera;

/// <summary>
/// デバッグ描画のレンダラー
/// </summary>
class DebugDrawRenderer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
private://構造体
	//GPUリソース
	struct GpuResource{
		DebugDrawVertexData* vertexData = nullptr;
		ComPtr<ID3D12Resource>vertexResource = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		uint32_t vertexCapacity = 0;

		uint32_t* indexData = nullptr;
		ComPtr<ID3D12Resource>indexResource = nullptr;
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		uint32_t indexCapacity = 0;
	};
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
	/// <param name="renderCamera">描画カメラ</param>
	void Draw(uint32_t instanceIndex, Camera* renderCamera);

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
private://メンバ関数
	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	/// <param name="gpuResource">gpuリソース</param>
	/// <param name="vertexCount">頂点数</param>
	void CreateVertexResource(GpuResource& gpuResource, uint32_t vertexCount);

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	/// <param name="gpuResource">gpuリソース</param>
	/// <param name="indexCount">インデックス数</param>
	void CreateIndexResource(GpuResource& gpuResource, uint32_t indexCount);

	/// <summary>
	/// GPUリソースの生成
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void CreateGpuResource(const DebugDrawRenderData& renderData);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//デバッグ描画のレンダーデータ
	std::vector<DebugDrawRenderData>renderDatas_;
	//GPUリソース
	std::vector<GpuResource>gpuResources_;
};

