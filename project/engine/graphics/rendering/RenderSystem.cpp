#include "RenderSystem.h"
#include "DirectXBase.h"
#include "PipelineManager.h"
#include "LightingManager.h"
#include "Object3dRenderer.h"
#include "SkyBoxRenderer.h"
#include "DebugDrawRenderer.h"

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
	//デバッグ描画のレンダラー
	debugDrawRenderer_ = DebugDrawRenderer::Create(directXBase);
}

//描画
void RenderSystem::Draw(){
	//Object3d
	for (uint32_t i = 0; i < object3dRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(object3dRenderer_->GetBlendMode(i), PipelineType::kObject3d);
		//ライティングの設定
		lightingManager_->DrawSetting();
		//3dオブジェクトの描画
		object3dRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	object3dRenderer_->Reset();

	//デバッグ描画
	for (uint32_t i = 0; i < debugDrawRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(debugDrawRenderer_->GetBlendMode(i));
		//デバッグ描画の描画
		debugDrawRenderer_->Draw(i);
	}
	//デバッグ描画のリセット
	debugDrawRenderer_->Reset();

	//スカイボックス
	for (uint32_t i = 0; i < skyBoxRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(skyBoxRenderer_->GetBlendMode(i), PipelineType::kSkyBox);
		//スカイボックスの描画
		skyBoxRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	skyBoxRenderer_->Reset();

}

//Object3dのレンダラーの取得
Object3dRenderer* RenderSystem::GetObject3dRenderer(){
	return object3dRenderer_.get();
}

//SkyBoxのレンダラーの取得
SkyBoxRenderer* RenderSystem::GetSkyBoxRenderer(){
	return skyBoxRenderer_.get();
}

//デバッグ描画のレンダラーの取得
DebugDrawRenderer* RenderSystem::GetDebugDrawRenderer(){
	return debugDrawRenderer_.get();
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

//描画開始
void RenderSystem::PreDraw(BlendMode blendMode){
	//パイプラインのセットを取得
	PipelineSet pipelineSet = pipelineManager_->GetPipelineSet(PipelineType::kDebugDraw);
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//PSO
	ID3D12PipelineState* pso = pipelineSet.graphicsPipelineStates[static_cast<uint32_t>(blendMode)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
}
