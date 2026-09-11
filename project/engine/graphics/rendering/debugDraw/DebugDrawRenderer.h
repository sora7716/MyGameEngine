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
		//頂点
		Vector4* vertexData = nullptr;
		ComPtr<ID3D12Resource>vertexResource = nullptr;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		uint32_t vertexCapacity = 0;

		//インデックス
		uint32_t* indexData = nullptr;
		ComPtr<ID3D12Resource>indexResource = nullptr;
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		uint32_t indexCapacity = 0;

		//マテリアル
		Vector4* materialData = {};
		ComPtr<ID3D12Resource>materialResource = nullptr;

		//ワールド行列
		ComPtr<ID3D12Resource>worldMatrixResource = nullptr;
		Matrix4x4* worldMatrixData = nullptr;
	};
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class RenderSystem;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタKey</param>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<DebugDrawRenderer>Create(ConstructorKey key, DirectXBase* directXBase);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit DebugDrawRenderer(ConstructorKey);

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
private://メンバ関数
	//コピーコンストラクタ禁止
	DebugDrawRenderer(const DebugDrawRenderer&) = delete;
	//代入演算子の禁止
	DebugDrawRenderer operator=(const DebugDrawRenderer&) = delete;

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
	/// マテリアルリソースの生成
	/// </summary>
	/// <param name="gpuResource">gpuリソース</param>
	void CreateMaterialResource(GpuResource& gpuResource);

	/// <summary>
	/// ワールド行列リソースの生成
	/// </summary>
	/// <param name="gpuResource">gpuリソース</param>
	void CreateWorldMatrixResource(GpuResource& gpuResource);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//デバッグ描画のレンダーデータ
	std::vector<DebugDrawRenderData>renderDatas_;
	//GPUリソース
	std::vector<GpuResource>gpuResources_;
};

