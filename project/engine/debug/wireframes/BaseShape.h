#pragma once
#include "ResourceData.h"
#include "RenderingData.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <cstdint>
#include <memory>

//前方宣言
class DirectXBase;
class DirectXBase;
class TextureManager;
class Camera;
class GraphicsPipeline;

/// <summary>
/// 形
/// </summary>
namespace Primitive {
	class BaseShape {
	private://エイリアステンプレート
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	public://メンバ関数
		/// <summary>
		/// コンストラクタ
		/// </summary>
		BaseShape();

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~BaseShape();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="directXBase">DirectXの基盤部分</param>
		/// <param name="camera">カメラ</param>
		virtual void Initialize(DirectXBase* directXBase, Camera* camera);

		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update();

		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw();

		/// <summary>
		/// カメラのセッター
		/// </summary>
		/// <param name="camera">カメラ</param>
		void SetCamera(Camera* camera);

		/// <summary>
		/// カラーのセッター
		/// </summary>
		/// <param name="color">色</param>
		void SetColor(const Vector4& color);

		/// <summary>
		/// カラーのゲッター
		/// </summary>
		/// <returns>色</returns>
		Vector4 GetColor();
	protected://メンバ関数
		/// <summary>
		/// 頂点データの設定
		/// </summary>
		virtual void SettingVertexData() = 0;

		/// <summary>
		/// インデックスの設定
		/// </summary>
		virtual void SettingIndexData() = 0;
	private://メンバ関数
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
	private://メンバ変数
		//DirectXの基盤部分
		DirectXBase* directXBase_ = nullptr;
		//カメラ
		Camera* camera_ = nullptr;
		//バッファリソース
		ComPtr<ID3D12Resource>vertexResource_ = nullptr;//頂点
		ComPtr<ID3D12Resource>materialResource_ = nullptr;//マテリアル
		ComPtr<ID3D12Resource>indexResource_ = nullptr;//インデックス
		ComPtr<ID3D12Resource>wvpResource_ = nullptr;//ワールドビュープロジェクション
		//ワールドビュープロジェクションのデータ
		TransformationMatrix* wvpData_ = nullptr;
		//バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点
		D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};//インデックス	

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
		//頂点シェーダBlob
		ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
		//ピクセルシェーダBlob
		ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
		//グラフィックスパイプライン(PSO)
		ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
		std::unique_ptr<GraphicsPipeline> makeGraphicsPipeline_ = nullptr;
		//ファイル名
		std::wstring vertexShaderFileName_ = L"Shape.VS.hlsl";//頂点
		std::wstring pixelShaderFileName_ = L"Shape.PS.hlsl";//ピクセル
	protected://メンバ変数
		//頂点数
		int32_t vertexCount_ = 0;
		//インデックス数
		int32_t indexCount_ = 0;
		//ワールド行列
		Matrix4x4 worldMatrix_ = {};
		//ワールド座標
		Transform transform_ = {};
		//バッファリソース内のデータを指すポインタ
		Vector4* color_ = nullptr;
		//頂点データ
		VertexData* vertexData_ = nullptr;
		//インデックスデータ
		uint32_t* indexData_ = nullptr;
	};
}

