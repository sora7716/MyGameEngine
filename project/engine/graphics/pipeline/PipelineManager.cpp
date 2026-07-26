#include "PipelineManager.h"
#include "GraphicsPipeline.h"
#include "Blend.h"

//テーブルの初期化
void(PipelineManager::* PipelineManager::createPSOTable[])() = {
	&CreatePSOForObject3d,
	&CreatePSOForSprite,
	&CreatePSOForParticle,
	&CreatePSOForSkyBox,
};

//デストラクタ
PipelineManager::~PipelineManager(){
}

//初期化
void PipelineManager::Initialize(DirectXBase* directXBase){
	//ブレンド
	blend_ = std::make_unique<Blend>();
	//グラフィックスパイプライン
	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	//DirectXを記録
	graphicsPipeline_->SetDirectXBase(directXBase);
}

//PSOの作成
void PipelineManager::CreatePSO(){
	for (uint32_t i = 0; i < static_cast<uint32_t>(PiplineType::kPiplineTypeCount); i++){
		(this->*createPSOTable[i])();
	}
}

//パイプラインセットの取得
const PipelineSet& PipelineManager::GetPipelineSet(PiplineType pipelineSetType) const{
	// TODO: return ステートメントをここに挿入します
	uint32_t index = static_cast<uint32_t>(pipelineSetType);
	return pipelineSets_[index];
}

//オブジェクト3D用のPSO
void PipelineManager::CreatePSOForObject3d(){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"Object3d.VS.hlsl");
	graphicsPipeline_->SetPixelShaderFileName(L"Object3d.PS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceForObject3d();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForObject3d();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState(FillMode::kSolid);
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++){
		//ブレンドステート
		graphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		pipelineSets_[static_cast<uint32_t>(PiplineType::kObject3d)].graphicsPipelineStates[i] = graphicsPipeline_->CreateGraphicsPipeline();
	}	//ルートシグネイチャの記録
	pipelineSets_[static_cast<uint32_t>(PiplineType::kObject3d)].rootSignature = graphicsPipeline_->GetRootSignature();
}

//PSOの作成(Sprite)
void PipelineManager::CreatePSOForSprite(){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"Sprite.VS.hlsl");
	graphicsPipeline_->SetPixelShaderFileName(L"Sprite.PS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateRootSignatureBlobForSprite();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForSprite();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState();
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++){
		//ブレンドステート
		graphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		pipelineSets_[static_cast<uint32_t>(PiplineType::kSprite)].graphicsPipelineStates[i] = graphicsPipeline_->CreateGraphicsPipeline();
	}
	//ルートシグネイチャの記録
	pipelineSets_[static_cast<uint32_t>(PiplineType::kSprite)].rootSignature = graphicsPipeline_->GetRootSignature();
}

//PSOの作成(Particle)
void PipelineManager::CreatePSOForParticle(){
	graphicsPipeline_->SetVertexShaderFileName(L"Particle.VS.hlsl");
	graphicsPipeline_->SetPixelShaderFileName(L"Particle.PS.hlsl");
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForParticle();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState();
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader();
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceForParticle();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++){
		//ブレンドステート
		graphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成[
		pipelineSets_[static_cast<uint32_t>(PiplineType::kParticle)].graphicsPipelineStates[i] = graphicsPipeline_->CreateGraphicsPipeline();
	}	//ルートシグネイチャの記録
	pipelineSets_[static_cast<uint32_t>(PiplineType::kParticle)].rootSignature = graphicsPipeline_->GetRootSignature();
}

//PSOの作成(SkyBox)
void PipelineManager::CreatePSOForSkyBox(){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"SkyBox.VS.hlsl");
	graphicsPipeline_->SetPixelShaderFileName(L"SkyBox.PS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceForParticle();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForSkyBox();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDescForSkyBox();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState();
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++){
		//ブレンドステート
		graphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		pipelineSets_[static_cast<uint32_t>(PiplineType::kSkyBox)].graphicsPipelineStates[i] = graphicsPipeline_->CreateGraphicsPipeline();
	}
	//ルートシグネイチャの記録
	pipelineSets_[static_cast<uint32_t>(PiplineType::kSkyBox)].rootSignature = graphicsPipeline_->GetRootSignature();
}

//コンストラクタ
PipelineManager::PipelineManager(ConstructorKey){
}
