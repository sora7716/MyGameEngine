#include "DebugDrawRenderer.h"
#include "DirectXBase.h"
#include "Camera.h"
#include <cassert>
#include <cstring>

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
	renderDatas_.push_back(renderData);

	//インスタンスの検索キー
	const uint32_t instanceIndex = static_cast<uint32_t>(renderDatas_.size() - 1);

	//gpuリソースの追加
	if (renderDatas_.size() > gpuResources_.size()){
		CreateGpuResource(renderData);
	}

	GpuResource& gpuResource = gpuResources_[instanceIndex];

	//頂点数を保存
	const uint32_t kVertexCount = static_cast<uint32_t>(renderData.vertices_.size());
	//インデックス数を保存
	const uint32_t kIndexCount = static_cast<uint32_t>(renderData.indices_.size());

	if (kVertexCount > gpuResource.vertexCapacity){
		//頂点を作り直し
		CreateVertexResource(gpuResource, kVertexCount);
	}

	if (kIndexCount > gpuResource.indexCapacity){
		//インデックスを作り直し
		CreateIndexResource(gpuResource, kIndexCount);
	}

	if (kVertexCount > 0){
		//頂点データにコピー
		std::memcpy(gpuResource.vertexData, renderData.vertices_.data(), sizeof(DebugDrawVertexData) * renderData.vertices_.size());
	}

	if (kIndexCount > 0){
		//インデックスデータにコピー
		std::memcpy(gpuResource.indexData, renderData.indices_.data(), sizeof(uint32_t) * renderData.indices_.size());
	}
}

//描画
void DebugDrawRenderer::Draw(uint32_t instanceIndex, Camera* renderCamera){
	//デバッグ描画の描画データ
	const DebugDrawRenderData& renderData = renderDatas_[instanceIndex];
	//GPUリソース
	GpuResource& gpuResource = gpuResources_[instanceIndex];
	//カメラ
	renderCamera->DrawSetting(2);
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, renderData.wvpResource->GetGPUVirtualAddress());//wvp
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&gpuResource.indexBufferView);//IBVを設定
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &gpuResource.vertexBufferView);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, renderData.materialResource->GetGPUVirtualAddress());//material
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(renderData.indices_.size()), 1, 0, 0, 0);
}

//描画データのリセット
void DebugDrawRenderer::Reset(){
	renderDatas_.clear();
}

//描画データのサイズの取得
uint32_t DebugDrawRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(renderDatas_.size());
}

//ブレンドモードの取得
BlendMode DebugDrawRenderer::GetBlendMode(uint32_t instanceIndex){
	return renderDatas_[instanceIndex].blendMode;
}

//頂点データの生成
void DebugDrawRenderer::CreateVertexResource(GpuResource& gpuResource, uint32_t vertexCount){
	//頂点リソースを生成
	gpuResource.vertexResource = directXBase_->CreateBufferResource(sizeof(DebugDrawVertexData) * vertexCount);
	//リソースの先頭アドレスから使う
	gpuResource.vertexBufferView.BufferLocation = gpuResource.vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	gpuResource.vertexBufferView.SizeInBytes = UINT(sizeof(DebugDrawVertexData) * vertexCount);
	//1頂点当たりのサイズ
	gpuResource.vertexBufferView.StrideInBytes = sizeof(DebugDrawVertexData);

	//頂点リソースにデータを書き込む
	gpuResource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.vertexData));//書き込むためのアドレスを取得

	//頂点のキャパシティを取得
	gpuResource.vertexCapacity = vertexCount;
}


//インデックスリソースの生成
void DebugDrawRenderer::CreateIndexResource(GpuResource& gpuResource, uint32_t indexCount){
	//インデックスリソースの生成
	gpuResource.indexResource = directXBase_->CreateBufferResource(sizeof(uint32_t) * indexCount);

	//リソースの先頭アドレスから使う
	gpuResource.indexBufferView.BufferLocation = gpuResource.indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	gpuResource.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indexCount);
	//1頂点当たりのサイズ
	gpuResource.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//データを書き込む
	gpuResource.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.indexData));

	//インデックスのキャパシティを取得
	gpuResource.indexCapacity = indexCount;
}

//GPUリソースの生成
void DebugDrawRenderer::CreateGpuResource(const DebugDrawRenderData& renderData){
	GpuResource gpuResource = {};

	//頂点リソースの生成
	CreateVertexResource(gpuResource, static_cast<uint32_t>(renderData.vertices_.size()));
	//頂点データにコピー
	std::memcpy(gpuResource.vertexData, renderData.vertices_.data(), sizeof(DebugDrawVertexData) * renderData.vertices_.size());

	//インデックスリソースの生成
	CreateIndexResource(gpuResource, static_cast<uint32_t>(renderData.indices_.size()));
	//インデックスデータにコピー
	std::memcpy(gpuResource.indexData, renderData.indices_.data(), sizeof(uint32_t) * renderData.indices_.size());

	//GPUリソースに追加
	gpuResources_.push_back(std::move(gpuResource));

}
