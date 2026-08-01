#include "RenderSystem.h"
#include "PipelineManager.h"
#include "DirectXBase.h"
#include "Object3dRenderer.h"

//コンストラクタ
RenderSystem::RenderSystem(){
}

//デストラクタ
RenderSystem::~RenderSystem(){
}

//初期化
void RenderSystem::Initialize(DirectXBase* directXBase, SRVManager* srvManager, PipelineManager* pipelineManager){
	//DirectXの基盤部分の記録
	directXBase_ = directXBase;
	//パイプラインの管理の記録
	pipelineManager_ = pipelineManager;
	//Object3dのレンダラー
	object3dRenderer_ = Object3dRenderer::Create(directXBase, srvManager);
}

//描画開始
void RenderSystem::PreDraw(){
	//パイプラインのセットを取得
	PipelineSet pipelineSet = pipelineManager_->GetPipelineSet(PiplineType::kObject3d);
	//ブレンドモード
	BlendMode blendMode = BlendMode::kNormal;
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//PSO
	ID3D12PipelineState* pso = pipelineSet.graphicsPipelineStates[static_cast<uint32_t>(blendMode)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
}

//描画
void RenderSystem::Draw(){
	//3dオブジェクトの描画
	object3dRenderer_->Draw();
	object3dRenderer_->Reset();
}

//Object3dのレンダラーの取得
Object3dRenderer* RenderSystem::GetObject3dRenderer(){
	return object3dRenderer_.get();
}
