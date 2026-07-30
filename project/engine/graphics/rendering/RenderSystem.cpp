#include "RenderSystem.h"
#include "PipelineManager.h"
#include "DirectXBase.h"

//コンストラクタ
RenderSystem::RenderSystem(){
}

//デストラクタ
RenderSystem::~RenderSystem(){
}

//初期化
void RenderSystem::Initialize(DirectXBase* directXBase, PipelineManager* pipelineManager){
	//DirectXの基盤部分の記録
	directXBase_ = directXBase;
	//パイプラインの管理の記録
	pipelineManager_ = pipelineManager;
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
