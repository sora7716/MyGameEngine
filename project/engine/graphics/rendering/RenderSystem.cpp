#include "RenderSystem.h"
#include "DirectXBase.h"
#include "PipelineManager.h"
#include "LightingManager.h"
#include "Object3dRenderer.h"
#include "SkyBoxRenderer.h"

//コンストラクタ
RenderSystem::RenderSystem(){
}

//デストラクタ
RenderSystem::~RenderSystem(){
}

//初期化
void RenderSystem::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, PipelineManager* pipelineManager, LightingManager* lightingManager){
	//DirectXの基盤部分の記録
	assert(directXBase);
	directXBase_ = directXBase;
	//パイプラインの管理の記録
	assert(pipelineManager);
	pipelineManager_ = pipelineManager;
	//ライティングの管理の記録
	assert(lightingManager);
	lightingManager_ = lightingManager;
	//Object3dのレンダラー
	object3dRenderer_ = Object3dRenderer::Create(directXBase, srvManager, textureManager);
	//スカイボックスのレンダラー
	skyBoxRenderer_ = SkyBoxRenderer::Create(directXBase, textureManager);
}

//描画
void RenderSystem::Draw(){
	for (uint32_t i = 0; i < object3dRenderer_->GetRenderDataSize(); i++){
		PreDraw(object3dRenderer_->GetBlendMode(i), PipelineType::kObject3d);
		//ライティングの設定
		lightingManager_->DrawSetting();
		//3dオブジェクトの描画
		object3dRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	object3dRenderer_->Reset();
}

//Object3dのレンダラーの取得
Object3dRenderer* RenderSystem::GetObject3dRenderer(){
	return object3dRenderer_.get();
}

//描画開始
void RenderSystem::PreDraw(BlendMode blendMode, PipelineType pipelineType){
	//パイプラインのセットを取得
	PipelineSet pipelineSet = pipelineManager_->GetPipelineSet(pipelineType);
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//PSO
	ID3D12PipelineState* pso = pipelineSet.graphicsPipelineStates[static_cast<uint32_t>(blendMode)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
}