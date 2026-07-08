#pragma once
#include "Blend.h"
#include "engine/base/GraphicsPipelineData.h"
#include <dxcapi.h>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <string>

//前方宣言
class DirectXBase;

/// <summary>
/// グラフィックスパイプライン
/// </summary>
class GraphicsPipeline {
private://エイリアステンプレート
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GraphicsPipeline();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GraphicsPipeline();

	/// <summary>
	/// ルートシグネイチャBlobの生成(スプライト用)
	/// </summary>
	void CreateRootSignatureBlobForSprite();

	/// <summary>
	/// ルートシグネイチャBlobの生成(Object3d用)
	/// </summary>
	void CreateRootSignatureBlobForObject3d();

	/// <summary>
	/// ルートシグネイチャBlobの生成(Particle用)
	/// </summary>
	void CreateRootSignatureBlobForParticle();

	/// <summary>
	/// ルートシグネイチャの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// インプットレイアウトの初期化
	/// </summary>
	void InitializeInputLayoutDesc();

	/// <summary>
	/// スカイボックスで使用するインプットレイアウトの初期化
	/// </summary>
	void InitializeInputLayoutDescForSkyBox();

	/// <summary>
	/// インプットレイアウトの初期化(スプライト)
	/// </summary>
	void InitializeInputLayoutDescForSprite();

	/// <summary>
	/// ブレンドステートの初期化
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void InitializeBlendState(int32_t blendMode);

	/// <summary>
    /// ラスタライザステートの初期化
    /// </summary>
	/// <param name="fillMode">ポリゴンをどう描画するかの設定</param>
	void InitializeRasterizerSatate(FillMode fillMode = FillMode::kSolid);

	/// <summary>
	/// 頂点シェーダのコンパイル
	/// </summary>
	void CompileVertexShader();

	/// <summary>
	/// ピクセルシェーダのコンパイル
	/// </summary>
	void CompilePixelShader();

	/// <summary>
	/// PSOの生成
	/// </summary>
	ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(D3D12_DEPTH_STENCIL_DESC depthStencilDesc);

	/// <summary>
	/// ルートシグネイチャのゲッター
	/// </summary>
	/// <returns>ルートシグネイチャ</returns>
	ComPtr<ID3D12RootSignature>GetRootSignature();

	/// <summary>
	/// DirectXの基盤のセッター
	/// </summary>
	/// <param name="directXBase">DirectXの基盤部分</param>
	void SetDirectXBase(DirectXBase* directXBase);

	/// <summary>
	/// 頂点シェーダのファイル名をセット
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	void SetVertexShaderFileName(const std::wstring& fileName);

	/// <summary>
	/// ピクセルシェーダのファイル名をセット
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	void SetPixelShaderFileName(const std::wstring& fileName);
protected://メンバ変数
	//DirectXの基盤
	DirectXBase* directXBase_ = nullptr;
	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature>rootSignature_ = nullptr;
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
	//ファイル名
	std::wstring vertexShaderFileName_ = L"Object3d.VS.hlsl";//頂点
	std::wstring pixelShaderFileName_ = L"Object3d.PS.hlsl";//ピクセル
	//ルートシグネイチャBlob
	ComPtr<ID3DBlob>signatureBlob_ = nullptr;
};
