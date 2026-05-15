#pragma once
#include "ResourceData.h"
#include "RenderingData.h"
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

//線分
struct Segment {
	Vector3 origin;
	Vector3 diff;
};

/// <summary>
/// 形
/// </summary>
class Box {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Box();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Box();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="camera">カメラ</param>
	void Initialize(DirectXBase* directXBase, Camera* camera);

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

	/// <summary>
	/// カメラのセッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(Camera* camera);
private://メンバ関数
	/// <summary>
	/// 頂点データの設定
	/// </summary>
	void SettingVertexData();

	/// <summary>
	/// インデックスデータの設定
	/// </summary>
	void SettingIndexData();

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
	void InitializeRasterizerState();

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
private://定数
	//頂点数
	static inline const int32_t kVertexCount = 24;
	//インデックス数
	static inline const int32_t kIndexCount = 36;
	//ライトの最大値
	static inline const int32_t kMaxLightCount = 64;
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
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;//平行光源
	ComPtr<ID3D12Resource> pointLightResource_ = nullptr;//点光源
	ComPtr<ID3D12Resource> spotLightResource_ = nullptr;//スポットライト
	ComPtr<ID3D12Resource> cameraResource_ = nullptr;//カメラ
	//バッファリソース内のデータを指すポインタ
	Vector4* color_ = nullptr;
	//頂点データ
	VertexData* vertexData_ = nullptr;
	//インデックスデータ
	uint32_t* indexData_ = nullptr;
	//ワールドビュープロジェクションのデータ
	TransformationMatrix* wvpData_ = nullptr;
	//平行光源
	DirectionalLight* directionalLightPtr_ = nullptr;
	//点光源
	PointLight* pointLightPtr_ = nullptr;
	//スポットライト
	SpotLight* spotLightPtr_ = nullptr;
	//カメラ
	CameraForGPU* cameraForGPU_ = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス

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
	std::wstring vertexShaderFileName_ = L"Box.VS.hlsl";//頂点
	std::wstring pixelShaderFileName_ = L"Box.PS.hlsl";//ピクセル
	//頂点シェーダBlob
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	//ピクセルシェーダBlob
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	//グラフィックスパイプライン(PSO)
	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	//平行光源
	DirectionalLight directionalLightData_ = {};
	//点光源
	std::vector<PointLight> pointLightDataList_;
	//スポットライト
	std::vector<SpotLight>spotLightList_;

	////ブレンド
	//Blend* blend_ = nullptr;
	//BlendMode blendMode_ = BlendMode::kNone;

	//デフォルトカメラ
	Camera* defaultCamera_ = nullptr;

	//SRVインデックス
	uint32_t srvIndexPoint_ = 0;//PointLight
	uint32_t srvIndexSpot_ = 0;//SpotLight
};