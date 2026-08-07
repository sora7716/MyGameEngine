#include "SkyBoxRenderer.h"
#include "SkyBoxRenderer.h"
#include "Camera.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include <cassert>

//生成
std::unique_ptr<SkyBoxRenderer> SkyBoxRenderer::Create(DirectXBase* directXBase, TextureManager* textureManager){
	std::unique_ptr<SkyBoxRenderer>instance = std::make_unique<SkyBoxRenderer>();
	instance->Initialize(directXBase, textureManager);
	return instance;
}

//コンストラクタ
SkyBoxRenderer::SkyBoxRenderer(){
}

//デストラクタ
SkyBoxRenderer::~SkyBoxRenderer(){
}

//初期化
void SkyBoxRenderer::Initialize(DirectXBase* directXBase, TextureManager* textureManager){
	//DirectXの基盤部分の記録
	assert(directXBase);;
	directXBase_ = directXBase;
	//Textureの管理の記録
	assert(textureManager);
	textureManager_ = textureManager;
}

//レンダーデータの追加
void SkyBoxRenderer::AddRenderData(const SkyBoxRenderData& skyBoxRenderData){
	skyBoxRenderDatas_.push_back(skyBoxRenderData);
}

//リセット
void SkyBoxRenderer::Reset(){
	skyBoxRenderDatas_.clear();
}

//描画
void SkyBoxRenderer::Draw(uint32_t instanceIndex){
	//スカイボックスの描画データ
	SkyBoxRenderData skyBoxRenderData = skyBoxRenderDatas_[instanceIndex];

	//存在していなかったら
	if (!skyBoxRenderData.isActive){
		return;
	}
	//カメラ
	skyBoxRenderData.renderCamera->DrawSetting(3);
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, skyBoxRenderData.wvpResource->GetGPUVirtualAddress());
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &skyBoxRenderData.vertexBufferView);
	//IndexBufferViewを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&skyBoxRenderData.indexBufferView);
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, skyBoxRenderData.materialResource->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(skyBoxRenderData.imageFileName));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(skyBoxRenderData.indexCount, 1, 0, 0, 0);
}

//ブレンドモードの取得
BlendMode SkyBoxRenderer::GetBlendMode(uint32_t instanceIndex){
	return skyBoxRenderDatas_[instanceIndex].blendMode;
}

//描画データのサイズの取得
uint32_t SkyBoxRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(skyBoxRenderDatas_.size());
}
