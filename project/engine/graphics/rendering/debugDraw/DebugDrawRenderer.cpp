#include "DebugDrawRenderer.h"
#include "DirectXBase.h"
#include "Camera.h"
#include <cassert>

//生成
std::unique_ptr<DebugDrawRenderer> DebugDrawRenderer::Create(DirectXBase* directXBase){
	std::unique_ptr<DebugDrawRenderer>instance = std::make_unique<DebugDrawRenderer>();
	instance->Initialize(directXBase);
	return instance;
}

//コンストラクタ
DebugDrawRenderer::DebugDrawRenderer(){
}

//デストラクタ
DebugDrawRenderer::~DebugDrawRenderer(){
}

//初期化
void DebugDrawRenderer::Initialize(DirectXBase* directXBase){
	//DirectXの基盤部分の記録
	assert(directXBase);
	directXBase_ = directXBase;
}

//描画データの追加
void DebugDrawRenderer::AddRenderData(const DebugDrawRenderData& renderData){
	debugDrawRenderDatas_.push_back(renderData);
}

//描画
void DebugDrawRenderer::Draw(uint32_t instanceIndex){
	//デバッグ描画の描画データ
	DebugDrawRenderData renderData = debugDrawRenderDatas_[instanceIndex];
	//カメラ
	renderData.renderCamera->DrawSetting(2);
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, renderData.wvpResource->GetGPUVirtualAddress());//wvp
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&renderData.indexBufferView);//IBVを設定
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &renderData.vertexBufferView);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, renderData.materialResource->GetGPUVirtualAddress());//material
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(renderData.indexCount, 1, 0, 0, 0);
}

//描画データのリセット
void DebugDrawRenderer::Reset(){
	debugDrawRenderDatas_.clear();
}

//描画データのサイズの取得
uint32_t DebugDrawRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(debugDrawRenderDatas_.size());
}

//ブレンドモードの取得
BlendMode DebugDrawRenderer::GetBlendMode(uint32_t instanceIndex){
	return debugDrawRenderDatas_[instanceIndex].blendMode;
}
