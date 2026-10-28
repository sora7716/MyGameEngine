#include "GraphicsPipeline.h"
#include "DirectXBase.h"
#include "Logger.h"
#include "StringUtility.h"
#include "RootSignatureBuilder.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
using namespace Microsoft::WRL;
using namespace rasterizerMode;

//コンストラクタ
GraphicsPipeline::GraphicsPipeline(){}

//デストラクタ
GraphicsPipeline::~GraphicsPipeline(){}

//ルートシグネイチャBlobの生成(スプライト用)
void GraphicsPipeline::CreateRootSignatureBlobForSprite(){
	//ルートシグネイチャのビルダー
	std::unique_ptr<RootSignatureBuilder>builder = std::make_unique<RootSignatureBuilder>();
	builder->Initialize();

	//Samplerの設定
	builder->AddStaticSampler(ShaderType::kPixel, 0);
	//RootParameterの作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//色情報
	builder->AddCBV(ShaderType::kPixel, 0);
	//Transform
	builder->AddCBV(ShaderType::kVertex, 0);
	//テクスチャ情報
	builder->AddSRVTable(ShaderType::kPixel, 0);

	//構築
	rootSignature_ = builder->Build(directXBase_->GetDevice());
}

//ルートシグネイチャBlobの生成(SkyBox用)
void GraphicsPipeline::CreateRootSignatureBlobForSkyBox(){
	//ルートシグネイチャのビルダー
	std::unique_ptr<RootSignatureBuilder>builder = std::make_unique<RootSignatureBuilder>();
	builder->Initialize();

	//Samplerの設定
	builder->AddStaticSampler(ShaderType::kPixel, 0);
	//RootParameterの作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	//色情報
	builder->AddCBV(ShaderType::kPixel, 0);
	//Transform
	builder->AddCBV(ShaderType::kVertex, 0);
	//テクスチャ情報
	builder->AddSRVTable(ShaderType::kPixel, 0);
	//Camera
	builder->AddCBV(ShaderType::kVertex, 1);

	//構築
	rootSignature_ = builder->Build(directXBase_->GetDevice());
}

//ルートシグネイチャBlobの生成(Object3d用)
void GraphicsPipeline::CreateRootSignatureBlobForObject3d(){
	//ルートシグネイチャのビルダー
	std::unique_ptr<RootSignatureBuilder>builder = std::make_unique<RootSignatureBuilder>();
	builder->Initialize();

	//Samplerの設定
	builder->AddStaticSampler(ShaderType::kPixel, 0);
	////色情報
	builder->AddCBV(ShaderType::kPixel, 0);
	//Transform
	builder->AddSRVTable(ShaderType::kVertex, 0);
	//テクスチャ情報
	builder->AddSRVTable(ShaderType::kPixel, 0);
	//平行光源
	builder->AddCBV(ShaderType::kPixel, 1);
	//カメラ
	builder->AddCBV(ShaderType::kAll, 2);
	//点光源
	builder->AddSRVTable(ShaderType::kPixel, 1);
	//スポットライト
	builder->AddSRVTable(ShaderType::kPixel, 2);
	//リムライト
	builder->AddCBV(ShaderType::kPixel, 3);
	//環境マップ情報
	builder->AddSRVTable(ShaderType::kPixel, 3);

	//構築
	rootSignature_ = builder->Build(directXBase_->GetDevice());
}

//ルートシグネイチャBlobの生成(Particle用)
void GraphicsPipeline::CreateRootSignatureBlobForParticle(){
	//ルートシグネイチャのビルダー
	std::unique_ptr<RootSignatureBuilder>builder = std::make_unique<RootSignatureBuilder>();
	builder->Initialize();

	//Samplerの設定
	builder->AddStaticSampler(ShaderType::kPixel, 0);
	//色情報
	builder->AddCBV(ShaderType::kPixel, 0);
	//Transform
	builder->AddSRVTable(ShaderType::kVertex, 0);
	//テクスチャ
	builder->AddSRVTable(ShaderType::kPixel, 0);
	//カメラ
	builder->AddCBV(ShaderType::kVertex, 0);

	//構築
	rootSignature_ = builder->Build(directXBase_->GetDevice());
}

//ルートシグネイチャBlobの生成(DebugDraw)
void GraphicsPipeline::CreateRootSignatureBlobForWireframe(){
	//ルートシグネイチャのビルダー
	std::unique_ptr<RootSignatureBuilder>builder = std::make_unique<RootSignatureBuilder>();
	builder->Initialize();

	//RootParameterの作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//色情報
	builder->AddCBV(ShaderType::kPixel, 0);
	//Transform
	builder->AddCBV(ShaderType::kVertex, 0);
	//Camera
	builder->AddCBV(ShaderType::kVertex, 1);

	//構築
	rootSignature_ = builder->Build(directXBase_->GetDevice());
}

//インプットレイアウトの初期化
void GraphicsPipeline::InitializeInputLayoutDesc(){
	//InputElementDesc
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//InputLayout
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);
}

//スカイボックスで使用するインプットレイアウトの初期化
void GraphicsPipeline::InitializeInputLayoutDescForSkyBox(){
	//InputElementDesc
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//InputLayout
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);
}

//インプットレイアウトの初期化(スプライト)
void GraphicsPipeline::InitializeInputLayoutDescForSprite(){
	//InputElementDesc
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//InputLayout
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);
}

//インプットレイアウトの初期化(DebugDraw)
void GraphicsPipeline::InitializeInputLayoutDescForWireframe(){
	//InputElementDesc
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//InputLayout
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);
}

//ブレンドステートの初期化
void GraphicsPipeline::InitializeBlendState(BlendMode blendMode){
	//ブレンド関係のクラスの生成
	std::unique_ptr<Blend> blend = std::make_unique<Blend>();
	blendDesc_ = blend->SetBlendDesc(blendMode);
}

//ラスタライザステートの初期化
void GraphicsPipeline::InitializeRasterizerState(FillMode fillMode, CullingMode cullingMode){
	//現在のカリングモードの切り替え
	D3D12_CULL_MODE currentCullingMode = D3D12_CULL_MODE_NONE;
	if (cullingMode == CullingMode::kBack){
		currentCullingMode = D3D12_CULL_MODE_BACK;
	} else if (cullingMode == CullingMode::kFront){
		currentCullingMode = D3D12_CULL_MODE_FRONT;
	}
	//裏面(時計周り)を表示しない
	rasterizerDesc_.CullMode = currentCullingMode;

	//現在のフィルモードの切り替え
	D3D12_FILL_MODE currentFillMode = D3D12_FILL_MODE_SOLID;
	if (fillMode == FillMode::kWireframe){
		currentFillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = currentFillMode;
}

//頂点シェーダのコンパイル
void GraphicsPipeline::CompileVertexShader(){
	//VertexShader
	vertexShaderBlob_ = directXBase_->CompilerShader(L"engine/resources/shaders/vertex/" + vertexShaderFileName_, L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);
}

//ピクセルシェーダのコンパイル
void GraphicsPipeline::CompilePixelShader(){
	//PixelShader
	pixelShaderBlob_ = directXBase_->CompilerShader(L"engine/resources/shaders/pixel/" + pixelShaderFileName_, L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

//PSOの生成
ComPtr<ID3D12PipelineState> GraphicsPipeline::CreateGraphicsPipeline(){
	HRESULT result = S_FALSE;
	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	//InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;
	//VertexShader
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	//PixelShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	//BlendState
	graphicsPipelineStateDesc.BlendState = blendDesc_;
	//RasterizerState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくてよい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState>graphicsPipelineState;
	result = directXBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(result));
	return graphicsPipelineState;
}

//PSOの生成(DebugDraw)
ComPtr<ID3D12PipelineState> GraphicsPipeline::CreateGraphicsPipelineForWireframe(){
	HRESULT result = S_FALSE;
	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	//InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;
	//VertexShader
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	//PixelShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	//BlendState
	graphicsPipelineStateDesc.BlendState = blendDesc_;
	//RasterizerState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	//どのように画面に色を打ち込むかの設定(気にしなくてよい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState>graphicsPipelineState;
	result = directXBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(result));
	return graphicsPipelineState;
}

//深度バッファの生成(ReadWrite)
void GraphicsPipeline::CreateDepthStencilResourceReadWrite(){
	//深度バッファを取得
	depthStencilResource_ = directXBase_->GetDepthStencilTexture();
	//Depthの機能を有効化
	depthStencilDesc_.DepthEnable = true;
	//書き込みをする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

//深度バッファの生成(パーティクル)
void GraphicsPipeline::CreateDepthStencilResourceReadOnly(){
	//深度バッファを取得
	depthStencilResource_ = directXBase_->GetDepthStencilTexture();
	//Depthの機能を有効化
	depthStencilDesc_.DepthEnable = true;
	//書き込みをする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

//ルートシグネイチャのゲッター
ComPtr<ID3D12RootSignature> GraphicsPipeline::GetRootSignature(){
	return rootSignature_;
}

//デプスステンシルの取得
D3D12_DEPTH_STENCIL_DESC GraphicsPipeline::GetDepthStencilDesc(){
	return depthStencilDesc_;
}

//DirectXの基盤のセッター
void GraphicsPipeline::SetDirectXBase(DirectXBase* directXBase){
	directXBase_ = directXBase;
}

//頂点シェーダのファイル名をセット
void GraphicsPipeline::SetVertexShaderFileName(const std::wstring& fileName){
	vertexShaderFileName_ = fileName;
}

//ピクセルシェーダのファイル名をセット
void GraphicsPipeline::SetPixelShaderFileName(const std::wstring& fileName){
	pixelShaderFileName_ = fileName;
}