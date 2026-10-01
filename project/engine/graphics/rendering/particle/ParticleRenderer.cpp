#include "ParticleRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "Model.h"
#include "MaterialInstance.h"
#include "ParticleEmitter.h"
#include "Camera.h"
#include "Object3dRenderData.h"
#include <cassert>

//生成
std::unique_ptr<ParticleRenderer> ParticleRenderer::Create(ConstructorKey key, DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager){
	std::unique_ptr<ParticleRenderer>instance = std::make_unique<ParticleRenderer>(key);
	instance->Initialize(directXBase, srvManager, textureManager);
	return instance;
}

//コンストラクタ
ParticleRenderer::ParticleRenderer(ConstructorKey){
}

//デストラクタ
ParticleRenderer::~ParticleRenderer(){
	//SRVの解放
	for (const GpuResource& gpuResource : gpuResources_){
		srvManager_->Free(gpuResource.srvIndex);
	}
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
void ParticleRenderer::Draw(uint32_t instanceIndex, Camera* renderCamera){
	//インスタンスの検索キーを確認
	assert(renderDatas_.size() > instanceIndex);
	assert(gpuResources_.size() > instanceIndex);

	//各インスタンスごとの描画データ
	const ParticleRenderData& renderData = renderDatas_[instanceIndex];

	//GPUリソースを取得
	GpuResource& gpuResource = gpuResources_[instanceIndex];

	//マテリアルインスタンスのスロットを取得
	const std::vector <MaterialInstanceSlot>& slots = renderData.materialInstance->GetSlots();
	//サイズを確認
	assert(gpuResource.materialDatas.size() == slots.size());

	for (uint32_t i = 0; i < gpuResource.materialDatas.size(); i++){
		//マテリアルを適応
		*gpuResource.materialDatas[i] = slots[i].material;
	}

	//インスタンスデータの適応
	uint32_t drawCount = UpdateParticleInstance(gpuResource, renderData, renderCamera);

	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(gpuResource.srvIndex));

	for (uint32_t i = 0; i < renderData.model->GetMeshes().size(); i++){
		//メッシュを取得
		const std::unique_ptr<Mesh>& mesh = renderData.model->GetMeshes()[i];

		//メッシュの描画データ
		const MeshRenderData& meshRenderData = mesh->GetMeshRenderData();

		//マテリアルインデックス
		uint32_t materialIndex = mesh->GetMaterialIndex();

		//範囲の確認
		assert(materialIndex < slots.size());
		assert(materialIndex < gpuResource.materialResources.size());

		//テクスチャパスを適応
		const std::string& materialTexturePath = slots[materialIndex].texturePaths.textureFilePath;

		//VertexBufferViewの設定
		directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &meshRenderData.vertexBufferView);

		//IndexBufferViewの設定
		directXBase_->GetCommandList()->IASetIndexBuffer(&meshRenderData.indexBufferView);

		//マテリアルCBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, gpuResource.materialResources[materialIndex]->GetGPUVirtualAddress());

		//SRVのDescriptorTableの先頭を設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(materialTexturePath.c_str()));

		if (drawCount > 0){
			//描画
			directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(meshRenderData.meshData.indices.size()), drawCount, 0, 0, 0);
		}
	}
}

//リセット
void ParticleRenderer::Reset(){
	renderDatas_.clear();
}

//描画データの追加
void ParticleRenderer::AddRenderData(const ParticleRenderData& renderData){
	//Nullチェック
	assert(renderData.model);
	assert(renderData.materialInstance);

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
		CreateMaterialResources(gpuResource, static_cast<uint32_t>(renderData.model->GetModelData().materialTexturePaths.size()));
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
	gpuResource.materialDatas.resize(materialCount);
	gpuResource.materialResources.resize(materialCount);

	for (uint32_t i = 0; i < materialCount; i++){
		//マテリアル用のリソースを作る
		gpuResource.materialResources[i] = directXBase_->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレスを取得
		gpuResource.materialResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.materialDatas[i]));
		//色を書き込む
		gpuResource.materialDatas[i]->color = Vector4::GetWhiteColor();
		gpuResource.materialDatas[i]->enableLighting = true;
		gpuResource.materialDatas[i]->uvMatrix = Matrix4x4::Identity4x4();
		gpuResource.materialDatas[i]->shininess = 10.0f;
		gpuResource.materialDatas[i]->environmentCoefficient = 0.0f;
	}
}

//パーティクルのインスタンスの更新
uint32_t ParticleRenderer::UpdateParticleInstance(GpuResource& gpuResource, const ParticleRenderData& renderData, Camera* renderCamera){
	//パーティクルがあるか確認
	assert(renderData.particles);

	//描画する数
	uint32_t drawCount = 0;
	for (const Particle& particle : *renderData.particles){
		//パーティクルが表示されてなければ
		if (!particle.isEnabled){
			continue;
		}

		//インスタンスの検索キーがGPUの配列を超えないようにする
		if (drawCount >= gpuResource.capacity){
			break;
		}

		//ワールド行列の作成
		Matrix4x4 worldMatrix = matrixUtility::MakeBillboardAffineMatrix(renderCamera->GetWorldMatrix(), particle.transform);

		//Gpuリソースに反映
		gpuResource.instanceData[drawCount].world = worldMatrix;
		gpuResource.instanceData[drawCount].color = particle.color;

		//インスタンスの検索キーを加算
		drawCount++;
	}

	return drawCount;
}
