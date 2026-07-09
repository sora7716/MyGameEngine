#pragma once
#include "RenderData.h"
#include "BlendMode.h"
#include "ParticleEmitter.h"
#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include <vector>

//前方宣言
class DirectXBase;
class ParticleCommon;
class Mesh;

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
	/// <param name="model">モデル</param>
	void Initialize(ParticleCommon* particleCommon, Camera* renderCamera, const std::string& textureName, Model* model = nullptr);

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
	/// カメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetGameCamera(Camera* camera);

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
private://メンバ関数

	/// <summary>
	/// メッシュデータの初期化
	/// </summary>
	/// <returns>メッシュデータ</returns>
	MeshData InitializePlaneModelData();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

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
	//メッシュ
	std::vector<std::unique_ptr<Mesh>>meshes_;
	//マテリアルデータ
	Material* materialData_ = nullptr;
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
