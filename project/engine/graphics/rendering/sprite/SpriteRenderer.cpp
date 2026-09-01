#include "SpriteRenderer.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include <cassert>
#include <cstring>

//生成
std::unique_ptr<SpriteRenderer> SpriteRenderer::Create(DirectXBase* directXBase, TextureManager* textureManager){
	std::unique_ptr<SpriteRenderer>instance = std::make_unique<SpriteRenderer>();
	instance->Initialize(directXBase, textureManager);
	return instance;
}

//コンストラクタ
SpriteRenderer::SpriteRenderer(){
}

//デストラクタ
SpriteRenderer::~SpriteRenderer(){
}

//初期化
void SpriteRenderer::Initialize(DirectXBase* directXBase, TextureManager* textureManager){
	//DirectXの基盤部分を記録
	assert(directXBase);
	directXBase_ = directXBase;

	//Textureの管理を記録
	assert(textureManager);
	textureManager_ = textureManager;

	//頂点リソースの生成
	CreateVertexResource();

	//インデックスリソースの生成
	CreateIndexResource();
}

//描画データの追加
void SpriteRenderer::AddRenderData(const SpriteRenderData& renderData){
	renderDatas_.push_back(renderData);


	if (gpuResources_.size() < renderDatas_.size()){
		//GPUリソースの生成
		CreateGpuResource();
	}
}

//描画
void SpriteRenderer::Draw(uint32_t instanceIndex){
	const SpriteRenderData& renderData = renderDatas_[instanceIndex];
	GPUResource& gpuResource = gpuResources_[instanceIndex];

	//描画データの情報をGPUリソースに反映
	//マテリアル
	*gpuResource.materialData = renderData.material;
	//トランスフォーメーション行列
	*gpuResource.transformationMatrix = renderData.transformationMatrix;

	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, gpuResource.transformationMatrixResource->GetGPUVirtualAddress());
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//IndexBufferViewを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, gpuResource.materialResource->GetGPUVirtualAddress());//material
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(renderData.imageFileName));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(kIndexCount, 1, 0, 0, 0);
}

//リセット
void SpriteRenderer::Reset(){
	renderDatas_.clear();
}

//ブレンドモードの取得
BlendMode SpriteRenderer::GetBlendMode(uint32_t instanceIndex){
	return renderDatas_[instanceIndex].blendMode;
}

//描画データの配列のサイズの取得
uint32_t SpriteRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(renderDatas_.size());
}

//頂点データの初期化
void SpriteRenderer::InitializeVertexData(){
	//要素数を設定
	vertices_.resize(kVertexCount);

	//頂点の初期化
	vertices_[0].position = { 0.0f,1.0f,0.0f,1.0f };//左下
	vertices_[0].texcoord = { 0.0f,1.0f };
	vertices_[0].normal = { 0.0f,0.0f,-1.0f };

	vertices_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertices_[1].texcoord = { 0.0f,0.0f };
	vertices_[1].normal = { 0.0f,0.0f,-1.0f };

	vertices_[2].position = { 1.0f,1.0f,0.0f,1.0f };//右下
	vertices_[2].texcoord = { 1.0f,1.0f };
	vertices_[2].normal = { 0.0f,0.0f,-1.0f };

	vertices_[3].position = { 1.0f,0.0f,0.0f,1.0f };//右上
	vertices_[3].texcoord = { 1.0f,0.0f };
	vertices_[3].normal = { 0.0f,0.0f,-1.0f };
}

//頂点データの生成
void SpriteRenderer::CreateVertexResource(){
	//VertexResourceを作成する
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * kVertexCount);
	//VertexBufferViewを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * kVertexCount;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点データ
	VertexData* vertexData = nullptr;
	//VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	InitializeVertexData();

	//配列の中身をポインタにコピー
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexData) * kVertexCount);
}

//インデックスデータの初期化
void SpriteRenderer::InitializeIndexData(){
	//要素数を設定
	indices_.resize(kIndexCount);

	//インデックスの初期化
	indices_[0] = 0; indices_[1] = 1; indices_[2] = 2;
	indices_[3] = 1; indices_[4] = 3; indices_[5] = 2;
}

//インデックスリソースの生成
void SpriteRenderer::CreateIndexResource(){
	//IndexResourceを作成する
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * kIndexCount);
	//IndexBufferViewを作成する
	//リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * kIndexCount;
	//インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスデータ
	uint32_t* indexData = nullptr;
	//IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	InitializeIndexData();

	//配列の中身をポインタにコピー
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * kIndexCount);
}

//マテリアルリソースの生成
void SpriteRenderer::CreateMaterialResource(GPUResource& gpuResource){
	//マテリアルリソースを作る
	gpuResource.materialResource = directXBase_->CreateBufferResource(sizeof(MaterialForSprite));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	gpuResource.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.materialData));
	//マテリアルデータの初期値を書き込む
	//色を書き込む
	gpuResource.materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	gpuResource.materialData->uvMatrix = Matrix4x4::Identity4x4();
}

//トランスフォーメーション行列リソースの生成
void SpriteRenderer::CreateTransformationMatrixResource(GPUResource& gpuResource){
	//座標変換行列リソースを作成する
	gpuResource.transformationMatrixResource = directXBase_->CreateBufferResource(sizeof(TransformationMatrixForSprite));
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	gpuResource.transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.transformationMatrix));
	//単位行列を書き込んでおく
	gpuResource.transformationMatrix->wvp = Matrix4x4::Identity4x4();
	gpuResource.transformationMatrix->world = Matrix4x4::Identity4x4();
}

//GPUリソースの生成
void SpriteRenderer::CreateGpuResource(){
	GPUResource gpuResource = {};
	//マテリアルのリソースの生成
	CreateMaterialResource(gpuResource);

	//トランスフォーメーション行列のリソースの生成
	CreateTransformationMatrixResource(gpuResource);
	gpuResources_.push_back(std::move(gpuResource));
}
