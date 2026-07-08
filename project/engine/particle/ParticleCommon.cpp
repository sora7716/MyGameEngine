#include "ParticleCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Blend.h"
#include <cassert>
using namespace Microsoft::WRL;

//デストラクタ
ParticleCommon::~ParticleCommon() {
	delete blend_;
	delete makeGraphicsPipeline_;
}

//初期化
void ParticleCommon::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager) {
	//DirectXの基盤を受け取る
	directXBase_ = directXBase;
	//SRVマネージャーを受け取る
	srvManager_ = srvManager;
	//テクスチャマネージャーを受け取る
	textureManager_ = textureManager;
	//ブレンド
	blend_ = new Blend();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = new GraphicsPipeline();
	//DirectXの基盤部分をセットする
	makeGraphicsPipeline_->SetDirectXBase(directXBase_);
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Particle.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Particle.PS.hlsl");
	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForParticle();
	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerState();
	//頂点シェーダBlob
	makeGraphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	makeGraphicsPipeline_->CompilePixelShader();
	//深度バッファ
	makeGraphicsPipeline_->CreateDepthStencilResourceForParticle();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++) {
		//ブレンドステート
		makeGraphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成[
		graphicsPipelineStates_[i] = makeGraphicsPipeline_->CreateGraphicsPipeline();
	}	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();
}

//共通描画設定
void ParticleCommon::DrawSetting() {
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//DirectXの基盤のゲッター
DirectXBase* ParticleCommon::GetDirectXBase() const {
	return directXBase_;
}

//SRVマネージャーのゲッター
SRVManager* ParticleCommon::GetSRVManager() const {
	return srvManager_;
}

//テクスチャマネージャーのゲッター
TextureManager* ParticleCommon::GetTextureManager() const {
	return textureManager_;
}

//グラフィックパイプラインのゲッター
std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> ParticleCommon::GetGraphicsPipelineStates() const {
	return graphicsPipelineStates_;
}

// デフォルトカメラのセッター
void ParticleCommon::SetDefaultCamera(Camera* camera) {
	defaultCamera_ = camera;
}

// デフォルトカメラのゲッター
Camera* ParticleCommon::GetDefaultCamera() const {
	return defaultCamera_;
}

//コンストラクタ
ParticleCommon::ParticleCommon(ConstructorKey) {
}
