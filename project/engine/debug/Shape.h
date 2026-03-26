#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include "engine/base/BlendMode.h"
#include <string>
#include <wrl.h>
#include <array>
#include <memory>
#include <d3d12.h>

//前方宣言
class DirectXBase;
class DirectXBase;
class TextureManager;
class GraphicsPipeline;
class Blend;
class Camera;

/// <summary>
/// 形
/// </summary>
class Shape {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Shape();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Shape();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="object2dCommon">2Dオブジェクトの共通部分</param>
	/// <param name="textureName">テクスチャのファイル名</param>
	void Initialize(DirectXBase* directXBase, TextureManager* textureManager, Camera* camera, const std::string& textureName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ
	/// </summary>
	void Debug();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
private://メンバ関数
	/// <summary>
	/// 頂点データの初期化
	/// </summary>
	void InitializeVertexData();

	/// <summary>
	/// 頂点リソースの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// インデックスリソースの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// マテリアルデータの初期化
	/// </summary>
	void InitializeMaterialData();

	/// <summary>
	/// マテリアルリソースの生成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// WorldTransformation行列リソースの生成
	/// </summary>
	void CreateTransformationMatrixResource();

	/// <summary>
	/// 座標の更新
	/// </summary>
	void UpdateTransform();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;
	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	//バッファリソース内のデータを指すポインタ
	Material* materialData_ = nullptr;//マテリアル
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//モデルデータ
	ModelData modelData_ = {};
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	
	//UV座標
	Transform2dData uvTransform_ = {
		.scale = { 1.0f,1.0f },
		.rotate = 0.0f,
		.translate = {0.0f,0.0f}
	};
	//ワールド座標
	TransformData transform_ = {};

	//ワールドビュープロジェクションのリソース
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;
	//ワールドビュープロジェクションのデータ
	TransformationMatrix* wvpData_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;
	//ワールド行列
	Matrix4x4 worldMatrix_ = {};
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//グラフィックスパイプライン(PSO)
	std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> graphicsPipelineStates_ = { nullptr };
	//グラフィックスパイプライン
	std::unique_ptr<GraphicsPipeline> makeGraphicsPipeline_ = nullptr;

	//ブレンド
	std::unique_ptr<Blend> blend_ = nullptr;
	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kNone;
};

