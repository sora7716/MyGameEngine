#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "ParticleEmitter.h"
#include <memory>
#include <d3d12.h>
#include <wrl.h>

//前方宣言
class DirectXBase;
class ParticleCommon;

/// <summary>
/// パーティクルシステム
/// </summary>
class ParticleSystem {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleSystem() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleSystem() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="particleCommon">パーティクルの共通部分</param>
	/// <param name="textureName">テクスチャ名</param>
	/// <param name="model">モデル</param>
	void Initialize(ParticleCommon* particleCommon, const std::string& textureName, Model* model = nullptr);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// テクスチャ名のゲッター
	/// </summary>
	/// <returns>テクスチャ名</returns>
	std::string GetTextureName();

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// ブレンドモードのセッター
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void SetBlendMode(BlendMode blendMode);

	/// <summary>
	/// トランスフォームデータのセッター
	/// </summary>
	/// <param name="transfrom">トランスフォーム</param>
	void SetTransformData(const Transform& transfrom);

	/// <summary>
	/// パーティクルの数のセッター
	/// </summary>
	/// <param name="cont">パーティクルの数</param>
	void SetParticleCount(uint32_t cont);

	/// <summary>
	/// 発生範囲のセッター
	/// </summary>
	/// <param name="range">範囲</param>
	void SetEmitRange(float range);

	/// <summary>
	/// 加速度が起こるフィールドのセッター
	/// </summary>
	/// <param name="field">フィールド</param>
	void SetAccelerationField(const AccelerationField& field);
private://メンバ関数
	/// <summary>
	/// モデルデータの初期化
	/// </summary>
	void InitializeQuadModelData();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// ワールドトランスフォームのリソースの生成
	/// </summary>
	void CreateWorldTransformResource();

	/// <summary>
	/// ストラクチャバッファの生成
	/// </summary>
	void CreateStructuredBuffer();
private://メンバ変数
	//DirectXの基盤部分	
	DirectXBase* directXBase_ = nullptr;
	//パーティクルの共通部分
	ParticleCommon* particleCommon_ = nullptr;
	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>instancingResource_ = nullptr;
	//ワールドビュープロジェクションのデータ
	ParticleForGPU* instancingData_ = {};
	//モデルデータ
	ModelData modelData_ = {};
	//マテリアルデータ
	Material* materialData_ = nullptr;
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	
	//SRVインデックス
	uint32_t srvIndex_ = 0;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kAdd;
	//パーティクルの発生源
	std::unique_ptr<ParticleEmitter>emitter_ = nullptr;
};
