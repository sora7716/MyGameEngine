#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Object3dRenderData.h"
#include "Object3dGpuResource.h"
#include <memory>
#include <cstdint>
#include <limits>

//前方宣言
class DirectXBase;
class SRVManager;
class TextureManager;
class Model;
class MaterialInstance;

/// <summary>
/// Object3dのレンダラー
/// </summary>
class Object3dRenderer{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
private://構造体など
	//オブジェクト3dのバッチリソース
	struct Object3dBatchResource{
		Model* model = nullptr;
		MaterialInstance* materialInstance = nullptr;
		BlendMode blendMode = BlendMode::kNone;
		Object3dRenderHandle handle = kInvalidObject3dRenderHandle;
		std::vector<ComPtr<ID3D12Resource>>materialResources;
		std::vector<Material*>materialPtrs;
		ComPtr<ID3D12Resource>rimLightResource = nullptr;
		RimLight* rimLightPtr = nullptr;
		uint64_t uploadedRevision = (std::numeric_limits<uint64_t>::max)();
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="maxInstance">インスタンスの最大値</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<Object3dRenderer>Create(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, uint32_t maxInstance = 1024);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Object3dRenderer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Object3dRenderer();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVの管理</param>
	/// <param name="textureManager">テクスチャの管理</param>
	/// <param name="maxInstance">インスタンスの最大値</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, uint32_t maxInstance);

	/// <summary>
	/// オブジェクトを登録
	/// </summary>
	/// <param name="lodCount">Lodの数</param>
	/// <param name="maxInstance">Object3dが表示される最大数</param>
	Object3dRenderHandle RegisterObject(uint32_t lodCount, uint32_t maxInstance);

	/// <summary>
	///リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="instanceIndex">インスタンス検索キー</param>
	void Draw(uint32_t instanceIndex);

	/// <summary>
	/// バッチを受け取る関数
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="materialInstance">マテリアルインスタンス</param>
	/// <param name="blendMode">ブレンドモード</param>
	/// <param name="transformations">トランスフォーメーションデータ</param>
	/// <param name="renderCamera">描画用カメラ</param>
	void SubmitBatch(Model* model, MaterialInstance* materialInstance, BlendMode blendMode, const std::vector<TransformationMatrix>& transformations, Camera* renderCamera);

	/// <summary>
	/// 描画データの追加
	/// </summary>
	/// <param name="renderData">描画データ</param>
	void AddRenderData(const Object3dRenderData& renderData);

	/// <summary>
	/// ブレンドモードの取得
	/// </summary>
	/// <param name="instanceIndex">インスタンス検索キー</param>
	/// <returns>ブレンドモード</returns>
	BlendMode GetBlendMode(uint32_t instanceIndex);

	/// <summary>
	/// 描画データの配列のサイズの取得
	/// </summary>
	/// <returns>描画データの配列のサイズ</returns>
	uint32_t GetRenderDataSize();
private://メンバ関数
	/// <summary>
	/// 座標変換行列リソースの生成
	/// </summary>
	/// <param name="lodGpuResource">LodのGpuに送るデータ</param>
	void CreateTransformationMatrixResource(LODGpuResource& lodGpuResource);

	/// <summary>
	/// 座標変換行列リソースのストラクチャバッファの生成
	/// </summary>
	/// <param name="lodGpuResource">LodのGpuに送るデータ</param>
	void CreateStructuredBufferForWvp(LODGpuResource& lodGpuResource);

	/// <summary>
    /// MaterialInstance用のGPUリソースを生成
    /// </summary>
	/// <param name="batchResource">バッチリソース</param>
	void CreateMaterialInstanceResource(Object3dBatchResource& batchResource);

	/// <summary>
	/// マテリアルインスタンスの更新
	/// </summary>
	/// <param name="batchResource">バッチリソース</param>
	void UpdateMaterialInstanceResource(Object3dBatchResource& batchResource);
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
	//テクスチャの管理
	TextureManager* textureManager_ = nullptr;
	//インスタンスの最大値
	uint32_t maxInstanceCount_ = 0;
	//マイフレーム消す描画データ
	std::vector<Object3dRenderData>renderDatas_;
	//Object3dが生存している間保持する
	std::vector<Object3dGpuResource>objectResources_;
	//バッチリソース
	std::vector<Object3dBatchResource>batchResources_;
};

