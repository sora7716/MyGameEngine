#include "DebugDrawRenderer.h"
#include "DirectXBase.h"
#include "Camera.h"
#include <cassert>
#include <cstring>

//生成
std::unique_ptr<DebugDrawRenderer> DebugDrawRenderer::Create(ConstructorKey key, DirectXBase* directXBase){
	std::unique_ptr<DebugDrawRenderer>instance = std::make_unique<DebugDrawRenderer>(key);
	instance->Initialize(directXBase);
	return instance;
}

//コンストラクタ
DebugDrawRenderer::DebugDrawRenderer(ConstructorKey){
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

	//GPUリソースのサイズとインスタンス検索キーを比べて
	if (gpuResources_.size() <= instanceIndex){
		//gpuリソースの追加
		GpuResource& gpuResource = gpuResources_.emplace_back();

		CreateMaterialResource(gpuResource);
		CreateWorldMatrixResource(gpuResource);
	}

	GpuResource& gpuResource = gpuResources_[instanceIndex];

	//頂点数を保存
	const uint32_t kVertexCount = static_cast<uint32_t>(renderData.vertices.size());
	//インデックス数を保存
	const uint32_t kIndexCount = static_cast<uint32_t>(renderData.indices.size());

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
		std::memcpy(gpuResource.vertexData, renderData.vertices.data(), sizeof(Vector4) * renderData.vertices.size());
	}

	if (kIndexCount > 0){
		//インデックスデータにコピー
		std::memcpy(gpuResource.indexData, renderData.indices.data(), sizeof(uint32_t) * renderData.indices.size());
	}
}

//描画
void DebugDrawRenderer::Draw(uint32_t instanceIndex){
	//デバッグ描画の描画データ
	const DebugDrawRenderData& renderData = renderDatas_[instanceIndex];
	//GPUリソース
	GpuResource& gpuResource = gpuResources_[instanceIndex];
	//描画データを適応
	//マテリアル
	*gpuResource.materialData = renderData.material;
	//ワールド行列
	*gpuResource.worldMatrixData = renderData.worldMatrix;
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, gpuResource.worldMatrixResource->GetGPUVirtualAddress());//wvp
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&gpuResource.indexBufferView);//IBVを設定
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &gpuResource.vertexBufferView);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, gpuResource.materialResource->GetGPUVirtualAddress());//material
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(renderData.indices.size()), 1, 0, 0, 0);
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
	HRESULT hr = S_FALSE;
	//頂点リソースを生成
	gpuResource.vertexResource = directXBase_->CreateBufferResource(sizeof(Vector4) * vertexCount);
	//リソースの先頭アドレスから使う
	gpuResource.vertexBufferView.BufferLocation = gpuResource.vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	gpuResource.vertexBufferView.SizeInBytes = UINT(sizeof(Vector4) * vertexCount);
	//1頂点当たりのサイズ
	gpuResource.vertexBufferView.StrideInBytes = sizeof(Vector4);

	//頂点リソースにデータを書き込む
	hr = gpuResource.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.vertexData));//書き込むためのアドレスを取得
	assert(SUCCEEDED(hr));
	assert(gpuResource.vertexData);

	//頂点のキャパシティを取得
	gpuResource.vertexCapacity = vertexCount;
}


//インデックスリソースの生成
void DebugDrawRenderer::CreateIndexResource(GpuResource& gpuResource, uint32_t indexCount){
	HRESULT hr = S_FALSE;
	//インデックスリソースの生成
	gpuResource.indexResource = directXBase_->CreateBufferResource(sizeof(uint32_t) * indexCount);

	//リソースの先頭アドレスから使う
	gpuResource.indexBufferView.BufferLocation = gpuResource.indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	gpuResource.indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indexCount);
	//1頂点当たりのサイズ
	gpuResource.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//データを書き込む
	hr = gpuResource.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.indexData));
	assert(SUCCEEDED(hr));
	assert(gpuResource.indexData);

	//インデックスのキャパシティを取得
	gpuResource.indexCapacity = indexCount;
}

//マテリアルリソースの生成
void DebugDrawRenderer::CreateMaterialResource(GpuResource& gpuResource){
	HRESULT hr = S_FALSE;
	//マテリアルリソースを作る
	gpuResource.materialResource = directXBase_->CreateBufferResource(sizeof(Vector4));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	hr = gpuResource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.materialData));
	assert(SUCCEEDED(hr));
	assert(gpuResource.materialData);

	//マテリアルデータの初期値を書き込む
	*gpuResource.materialData = Vector4::MakeWhiteColor();
}

//ワールド行列リソースの生成
void DebugDrawRenderer::CreateWorldMatrixResource(GpuResource& gpuResource){
	HRESULT hr = S_FALSE;
	//座標変換行列リソースを作成する
	gpuResource.worldMatrixResource = directXBase_->CreateBufferResource(sizeof(Matrix4x4));
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	hr = gpuResource.worldMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.worldMatrixData));
	assert(SUCCEEDED(hr));
	assert(gpuResource.worldMatrixData);
	//単位行列を書き込んでおく
	*gpuResource.worldMatrixData = Matrix4x4::Identity4x4();
}
