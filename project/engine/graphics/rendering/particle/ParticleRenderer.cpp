#include "ParticleRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "Model.h"
#include "ParticleEmitter.h"
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

	//ハンドルが登録済みか確認
	assert(renderData.renderHandle != kInvalidParticleRenderHandle);
	assert(renderData.renderHandle < gpuResources_.size());
	//GPUリソースを取得
	const GpuResource& particleResource = gpuResources_[renderData.renderHandle];

	//ワールド行列の更新
	//emitter_->UpdateWorldMatrix(instancingData_);
	//カメラ
	renderData.renderCamera->DrawSetting(3);
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &renderData.vertexBufferView);//VBVを設定
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&renderData.indexBufferView);//IBVを設定
	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(particleResource.srvIndex));
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
	renderDatas_.clear();
}

//描画データの追加
void ParticleRenderer::AddRenderData(const ParticleRenderData& renderData){
	//レンダーデータの追加
	renderDatas_.push_back(renderData);

	//インスタンスの検索キー
	const uint32_t instanceIndex = static_cast<uint32_t>(renderDatas_.size() - 1);

	//描画データとGPUデータのサイズを比べる
	if (gpuResources_.size() <= instanceIndex){
		GpuResource& gpuResource = gpuResources_.emplace_back();

		//パーティクルのキャパシティを設定
		gpuResource.capacity = ParticleEmitter::kNumMaxInstance;

		//リソースの生成
		CreateTransformationMatrixResource(gpuResource);
		CreateStructuredBufferForParticleGpu(gpuResource);
		CreateMaterialResources(gpuResource, renderData.model->GetModelData().materialTexturePaths.size());
	}
}

//描画データのサイズを取得
uint32_t ParticleRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(renderDatas_.size());
}

//ブレンドモードの取得
BlendMode ParticleRenderer::GetBlendMode(uint32_t instanceIndex){
	return renderDatas_[instanceIndex].blendMode;
}

//パーティクルを登録
ParticleRenderHandle ParticleRenderer::RegisterParticle(uint32_t maxInstance){
	//インスタンスの最大値が0より小さくないか
	assert(maxInstance > 0);

	//ハンドルを作成
	const ParticleRenderHandle handle = static_cast<ParticleRenderHandle>(gpuResources_.size());

	gpuResources_.emplace_back();

	GpuResource& particleResource = gpuResources_.back();

	//キャパシティを設定
	particleResource.capacity = maxInstance;

	//座標のリソースを作成
	CreateTransformationMatrixResource(particleResource);

	//ストラクチャバッファの作成
	CreateStructuredBufferForParticleGpu(particleResource);

	return handle;
}

//座標変換行列リソースの生成
void ParticleRenderer::CreateTransformationMatrixResource(GpuResource& gpuResource){
	//座標変換行列リソースを作成する	
	gpuResource.instancingResource = directXBase_->CreateBufferResource(sizeof(ParticleForGPU) * gpuResource.capacity);
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	gpuResource.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.instanceData));
	for (uint32_t i = 0; i < gpuResource.capacity; i++){
		//単位行列を書き込んでおく
		gpuResource.instanceData[i].world = Matrix4x4::Identity4x4();
		gpuResource.instanceData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 初期色を白に設定
	}
}

//インスタンシングリソースのストラクチャバッファの生成
void ParticleRenderer::CreateStructuredBufferForParticleGpu(GpuResource& gpuResource){
	//ストラクチャバッファを生成
	gpuResource.srvIndex = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		gpuResource.srvIndex,
		gpuResource.instancingResource.Get(),
		gpuResource.capacity,
		sizeof(ParticleForGPU)
	);
}

//マテリアルリソースの生成
void ParticleRenderer::CreateMaterialResources(GpuResource& gpuResource, uint32_t materialCount){
	//マテリアルのサイズ設定
	gpuResource.materialData.resize(materialCount);
	gpuResource.materialResources.resize(materialCount);

	for (uint32_t i = 0; i < materialCount; i++){
		//マテリアル用のリソースを作る
		gpuResource.materialResources[i] = directXBase_->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレスを取得
		gpuResource.materialResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.materialData[i]));
		//色を書き込む
		gpuResource.materialData[i]->color = Vector4::MakeWhiteColor();
		gpuResource.materialData[i]->enableLighting = true;
		gpuResource.materialData[i]->uvMatrix = Matrix4x4::Identity4x4();
		gpuResource.materialData[i]->shininess = 10.0f;
		gpuResource.materialData[i]->environmentCoefficient = 0.0f;
	}
}
