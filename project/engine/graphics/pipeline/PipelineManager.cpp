#include "PipelineManager.h"
#include "GraphicsPipeline.h"
#include "Blend.h"
#include "StringUtility.h"
#include <functional>
using namespace rasterizerMode;

//パイプラインの検索キーのハッシュを作成
size_t PipelineKeyHasher::operator()(const PipelineKey& key)const{
	return key.MakeHashKey();
}

//テーブルの初期化
void(PipelineManager::* PipelineManager::createPSOTable[])(const PipelineKey& key) = {
	&CreatePSOForObject3d,
	&CreatePSOForParticle,
	&CreatePSOForWireframe,
	&CreatePSOForSkyBox,
	&CreatePSOForSprite,
};

//生成
std::unique_ptr<PipelineManager> PipelineManager::Create(ConstructorKey key, DirectXBase* directXBase, ShaderManager* shaderManager){
	//生成
	std::unique_ptr<PipelineManager>instance = std::make_unique<PipelineManager>(key);
	//初期化
	instance->Initialize(directXBase, shaderManager);

	return instance;
}

//コンストラクタ
PipelineManager::PipelineManager(ConstructorKey){
}

//デストラクタ
PipelineManager::~PipelineManager(){
}

//初期化
void PipelineManager::Initialize(DirectXBase* directXBase, ShaderManager* shaderManager){
	//ブレンド
	blend_ = std::make_unique<Blend>();
	//グラフィックスパイプライン
	graphicsPipeline_ = GraphicsPipeline::Create(shaderManager);

	//DirectXを記録
	graphicsPipeline_->SetDirectXBase(directXBase);
}

//PSOの取得(存在しない場合は確保してキャッシュする)
PipelineSet* PipelineManager::GetPSO(const PipelineKey& key){
	//すでに存在したら
	if (pipelineSets_.contains(key)){
		return &pipelineSets_.at(key);
	}

	//ピクセルシェーダファイルパスがなければ
	if (key.pixelShaderFilePath.empty()){
		return nullptr;
	}

	//存在しない場合
	(this->*createPSOTable[static_cast<uint32_t>(key.pipelineType)])(key);
	return &pipelineSets_.at(key);
}

//オブジェクト3D用のPSO
void PipelineManager::CreatePSOForObject3d(const PipelineKey& key){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"Object3d.VS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceReadWrite();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForObject3d();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState(key.fillMode, key.cullingMode);
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader(stringUtility::ConvertString(key.pixelShaderFilePath));
	//PSO
	//ブレンドステート
	graphicsPipeline_->InitializeBlendState(key.blendMode);

	PipelineSet pipelineSet = {};
	//グラフィックスパイプラインの生成
	pipelineSet.graphicsPipelineState = graphicsPipeline_->CreateGraphicsPipeline();
	//ルートシグネイチャの記録
	pipelineSet.rootSignature = graphicsPipeline_->GetRootSignature();
	//パイプラインセットの配列に追加
	pipelineSets_.emplace(key, pipelineSet);
}

//PSOの作成(Sprite)
void PipelineManager::CreatePSOForSprite(const PipelineKey& key){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"Sprite.VS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateRootSignatureBlobForSprite();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForSprite();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState(key.fillMode, key.cullingMode);
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader(stringUtility::ConvertString(key.pixelShaderFilePath));
	//PSO
	//ブレンドステート
	graphicsPipeline_->InitializeBlendState(key.blendMode);

	PipelineSet pipelineSet = {};
	//グラフィックスパイプラインの生成
	pipelineSet.graphicsPipelineState = graphicsPipeline_->CreateGraphicsPipeline();
	//ルートシグネイチャの記録
	pipelineSet.rootSignature = graphicsPipeline_->GetRootSignature();
	//パイプラインセットの配列に追加
	pipelineSets_.emplace(key, pipelineSet);
}

//PSOの作成(Particle)
void PipelineManager::CreatePSOForParticle(const PipelineKey& key){
	graphicsPipeline_->SetVertexShaderFileName(L"Particle.VS.hlsl");
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForParticle();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState(key.fillMode, key.cullingMode);
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader(stringUtility::ConvertString(key.pixelShaderFilePath));
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceReadOnly();
	//PSO
	//ブレンドステート
	graphicsPipeline_->InitializeBlendState(key.blendMode);

	PipelineSet pipelineSet = {};
	//グラフィックスパイプラインの生成
	pipelineSet.graphicsPipelineState = graphicsPipeline_->CreateGraphicsPipeline();
	//ルートシグネイチャの記録
	pipelineSet.rootSignature = graphicsPipeline_->GetRootSignature();
	//パイプラインセットの配列に追加
	pipelineSets_.emplace(key, pipelineSet);
}

//PSOの作成(SkyBox)
void PipelineManager::CreatePSOForSkyBox(const PipelineKey& key){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"SkyBox.VS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceReadOnly();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForSkyBox();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDescForSkyBox();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState(key.fillMode, key.cullingMode);
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader(stringUtility::ConvertString(key.pixelShaderFilePath));
	//PSO
	//ブレンドステート
	graphicsPipeline_->InitializeBlendState(key.blendMode);

	PipelineSet pipelineSet = {};
	//グラフィックスパイプラインの生成
	pipelineSet.graphicsPipelineState = graphicsPipeline_->CreateGraphicsPipeline();
	//ルートシグネイチャの記録
	pipelineSet.rootSignature = graphicsPipeline_->GetRootSignature();
	//パイプラインセットの配列に追加
	pipelineSets_.emplace(key, pipelineSet);
}

//PSOの作成(Wireframe)
void PipelineManager::CreatePSOForWireframe(const PipelineKey& key){
	//シェーダを設定
	graphicsPipeline_->SetVertexShaderFileName(L"Wireframe.VS.hlsl");
	//深度バッファ
	graphicsPipeline_->CreateDepthStencilResourceReadOnly();
	//シグネイチャBlobの初期化
	graphicsPipeline_->CreateRootSignatureBlobForWireframe();
	//インプットレイアウト
	graphicsPipeline_->InitializeInputLayoutDescForWireframe();
	//ラスタライザステート
	graphicsPipeline_->InitializeRasterizerState(key.fillMode, key.cullingMode);
	//頂点シェーダBlob
	graphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	graphicsPipeline_->CompilePixelShader(stringUtility::ConvertString(key.pixelShaderFilePath));
	//PSO
	//ブレンドステート
	graphicsPipeline_->InitializeBlendState(key.blendMode);

	PipelineSet pipelineSet = {};
	//グラフィックスパイプラインの生成
	pipelineSet.graphicsPipelineState = graphicsPipeline_->CreateGraphicsPipelineForWireframe();
	//ルートシグネイチャの記録
	pipelineSet.rootSignature = graphicsPipeline_->GetRootSignature();
	//パイプラインセットの配列に追加
	pipelineSets_.emplace(key, pipelineSet);
}
