#include "ParticleRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Mesh.h"
#include <cassert>

//生成
std::unique_ptr<ParticleRenderer> ParticleRenderer::Create(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager){
	std::unique_ptr<ParticleRenderer>instance = std::make_unique<ParticleRenderer>();
	instance->Initialize(directXBase, srvManager, textureManager);
	return instance;
}

//コンストラクタ
ParticleRenderer::ParticleRenderer(){
}

//デストラクタ
ParticleRenderer::~ParticleRenderer(){
}

//初期化
void ParticleRenderer::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager){
	//DirectXの基盤部分の記録
	assert(directXBase);
	directXBase_ = directXBase;
	//SRVの管理の記録
	assert(srvManager);
	srvManager_ = srvManager;
	//Textureの管理の記録
	assert(textureManager);
	textureManager_ = textureManager;
}

//描画
void ParticleRenderer::Draw(uint32_t instanceIndex){
	//各インスタンスごとの描画データ
	ParticleRenderData renderData = renderDatas_[instanceIndex];
	//ワールド行列の更新
	//emitter_->UpdateWorldMatrix(instancingData_);
	//カメラ
	renderData.renderCamera->DrawSetting(3);
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &renderData.vertexBufferView);//VBVを設定
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&renderData.indexBufferView);//IBVを設定
	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(renderData.srvIndex));
	for (uint32_t i = 0; i < renderData.meshes.size(); i++){
		//マテリアルインデックス
		uint32_t materialIndex = renderData.meshes[i]->GetMaterialIndex();

		//マテリアルCBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, renderData.materialResources[materialIndex]->GetGPUVirtualAddress());

		//テクスチャパスを適応
		std::string& materialTexturePath = renderData.imageTexturePaths[materialIndex];
		//SRVのDescriptorTableの先頭を設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(materialTexturePath.c_str()));
		//メッシュの描画
		renderData.meshes[i]->Draw(renderData.numInstance);
	}
}

//リセット
void ParticleRenderer::Reset(){
}

//描画データの追加
void ParticleRenderer::AddRenderData(const ParticleRenderData& renderData){
	renderDatas_.push_back(renderData);
}

//描画データのサイズを取得
uint32_t ParticleRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(renderDatas_.size());
}

//ブレンドモードの取得
BlendMode ParticleRenderer::GetBlendMode(uint32_t instanceIndex){
	return renderDatas_[instanceIndex].blendMode;
}
