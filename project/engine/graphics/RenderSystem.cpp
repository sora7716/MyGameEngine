#include "RenderSystem.h"
#include "GraphicsPipeline.h"
#include "Blend.h"
using namespace Microsoft::WRL;

//コンストラクタ
RenderSystem::RenderSystem(){
}

//デストラクタ
RenderSystem::~RenderSystem(){
}

//初期化
void RenderSystem::Initialize(DirectXBase* directXBase){
	//ブレンド
	blend_ = new Blend();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = new GraphicsPipeline();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Object3d.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Object3d.PS.hlsl");
	//DirectXを記録
	makeGraphicsPipeline_->SetDirectXBase(directXBase);
	//深度バッファ
	makeGraphicsPipeline_->CreateDepthStencilResourceForObject3d();
	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForObject3d();
	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerState(FillMode::kSolid);
	//頂点シェーダBlob
	makeGraphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	makeGraphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++){
		//ブレンドステート
		makeGraphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		graphicsPipelineStates_[i] = makeGraphicsPipeline_->CreateGraphicsPipeline();
	}	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();

}
