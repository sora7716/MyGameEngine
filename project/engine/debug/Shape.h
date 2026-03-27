#pragma once
#include "engine/math/ResourceData.h"
#include "engine/math/RenderingData.h"
#include <string>
#include <wrl.h>
#include <memory>
#include <d3d12.h>
#include <dxcapi.h>
#include <cstdint>

//前方宣言
class DirectXBase;
class DirectXBase;
class TextureManager;
class Camera;

struct LineVertex {
	Vector4 position;
	Vector4 color;
};

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
	/// ルートシグネイチャBlobの生成
	/// </summary>
	void CreateRootSignatureBlob();

	/// <summary>
	/// ルートシグネイチャの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// インプットレイアウトの初期化
	/// </summary>
	void InitializeInputLayoutDesc();

	/// <summary>
	/// ラスタライザステートの初期化
	/// </summary>
	void InitializeRasterizerSatate();

	/// <summary>
	/// 頂点シェーダのコンパイル
	/// </summary>
	void CompileVertexShader();

	/// <summary>
	/// ピクセルシェーダのコンパイル
	/// </summary>
	void CompilePixelShader();

	/// <summary>
	/// ブレンドステートの初期化
	/// </summary>
	void InitializeBlendState();

	/// <summary>
	/// PSOの生成
	/// </summary>
	/// <returns></returns>
	ComPtr<ID3D12PipelineState> CreateGraphicsPipeline();

	/// <summary>
	/// グラフィックスパイプラインの構築
	/// </summary>
	void BuildGraphicsPipeline();

	/// <summary>
	/// 座標の更新
	/// </summary>
	void UpdateTransform();
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;
	//カメラ
	Camera* camera_ = nullptr;

	//バッファリソース
	ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
	ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
	ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
	ComPtr<ID3D12Resource>wvpResource_ = nullptr;//ワールドビュープロジェクション
	//バッファリソース内のデータを指すポインタ
	//モデルデータ
	ModelData modelData_ = {};
	//マテリアルデータ
	Material* materialData_ = nullptr;
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//ワールドビュープロジェクションのデータ
	TransformationMatrix* wvpData_ = nullptr;
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

	//ワールド行列
	Matrix4x4 worldMatrix_ = {};

	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
	//ルートシグネイチャBlob
	ComPtr<ID3DBlob>signatureBlob_ = nullptr;
	//インプットレイアウト
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_ = {};
	//ブレンドステート
	D3D12_BLEND_DESC blendDesc_ = {};
	//ラスタライザステート
	D3D12_RASTERIZER_DESC rasterizerDesc_ = {};
	//ファイル名
	std::wstring vertexShaderFileName_ = L"Shape.VS.hlsl";//頂点
	std::wstring pixelShaderFileName_ = L"Shape.PS.hlsl";//ピクセル
	//頂点シェーダBlob
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	//ピクセルシェーダBlob
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	//グラフィックスパイプライン(PSO)
	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
};

