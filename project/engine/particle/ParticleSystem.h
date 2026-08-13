#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "ParticleData.h"
#include "ParticleRenderData.h"
#include "PipelineManagerData.h"
#include "ParticleGpuResource.h"
#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include <vector>

//前方宣言
class DirectXBase;
class SRVManager;
class PipelineManager;
class TextureManager;
class Mesh;
class Camera;
class ParticleEmitter;
class ParticleRenderer;

/// <summary>
/// パーティクルシステム
/// </summary>
class ParticleSystem{
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleSystem();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleSystem();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="particleCommon">パーティクルの共通部分</param>
	/// <param name="renderCamera">描画用カメラ</param>
	/// <param name="textureName">テクスチャ名</param>
	void Initialize(DirectXBase* directXBase, SRVManager* srvManager, PipelineManager* pipelineManager, Camera* renderCamera, const std::string& textureName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// レンダラーを登録
	/// </summary>
	/// <param name="renderer">レンダラー</param>
	void RegisterToRenderer(ParticleRenderer* renderer);

	void DrawSetting();

	/// <summary>
	/// ゲームカメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetGameCamera(Camera* camera);

	/// <summary>
	/// 描画カメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetRenderCamera(Camera* camera);

	/// <summary>
	/// ブレンドモードの設定
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// エミッター位置の設定
	/// </summary>
	/// <param name="position">位置</param>
	void SetEmitterPosition(const Vector3& position);

	/// <summary>
	/// パーティクルの数の設定
	/// </summary>
	/// <param name="cont">パーティクルの数</param>
	void SetParticleCount(uint32_t cont);

	/// <summary>
	/// 発生範囲の設定
	/// </summary>
	/// <param name="range">範囲</param>
	void SetEmitRange(float range);

	/// <summary>
	/// 加速度が起こるフィールドの設定
	/// </summary>
	/// <param name="field">フィールド</param>
	void SetAccelerationField(const AccelerationField& field);

	/// <summary>
	/// パーティクルの発生感覚[秒]の設定
	/// </summary>
	/// <param name="frequency">発生感覚</param>
	void SetFrequency(float frequency);

	/// <summary>
	/// モデルデータの設定
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	void SetModelData(const ModelData& modelData);

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="meshIndex">メッシュ検索キー</param>
	/// <param name="textureFileName">画像のファイル名</param>
	void SetTexture(uint32_t meshIndex, const std::string& imageFileName);

	/// <summary>
	/// 描画データの取得
	/// </summary>
	/// <returns>描画データ</returns>
	const ParticleRenderData& GetRenderData();
private://メンバ関数
	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResources();
private://メンバ変数
	//DirectXの基盤部分	
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
	//パイプラインの管理
	PipelineManager* pipelineManager_ = nullptr;
	PipelineSet pipelineSet_ = {};
	//描画用のカメラ
	Camera* renderCamera_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	
	//パーティクルのGPUリソース
	ComPtr<ID3D12Resource>instancingResource_ = nullptr;
	//パーティクルのGPUリソースのデータ
	std::vector<ParticleForGPU> particleForGpuDatas_ = {};
	//モデルデータ
	ModelData modelData_ = {};
	//メッシュ
	std::vector<std::shared_ptr<Mesh>>meshes_;
	//マテリアルのリソース
	std::vector<ComPtr<ID3D12Resource>>materialResources_;
	//マテリアルデータ
	std::vector<Material*> materialPtrs_;
	//SRVインデックス
	uint32_t srvIndex_ = 0;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kAdd;
	//パーティクルの発生源
	std::unique_ptr<ParticleEmitter>emitter_ = nullptr;
	//描画データ
	ParticleRenderData renderData_ = {};
	//描画ハンドル
	ParticleRenderHandle renderHandle_ = kInvalidParticleRenderHandle;
};
