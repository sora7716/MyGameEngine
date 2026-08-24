#include "SpriteRenderer.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include <cassert>

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

	//マテリアルのリソースの生成
	CreateMaterialResource();

	//トランスフォーメーション行列のリソースの生成
	CreateTransformationMatrixResource();
}

//描画データの追加
void SpriteRenderer::AddRenderData(const SpriteRenderData& renderData){
	renderDatas_.push_back(renderData);
}

//描画
void SpriteRenderer::Draw(uint32_t instanceIndex){
	SpriteRenderData renderData = renderDatas_[instanceIndex];

	//描画データを反映
	//マテリアル
	*materialData_ = renderData.material;
	//トランスフォーメーション行列
	*transformationMatrix_ = renderData.transformationMatrix;

	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//IndexBufferViewを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(renderData.imageFileName));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(kIndexCount, 1, 0, 0, 0);
}

//頂点データの初期化
void SpriteRenderer::InitializeVertexData(){
	//矩形
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

	//VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertices_));
	InitializeVertexData();
}

//インデックスデータの初期化
void SpriteRenderer::InitializeIndexData(){
	indices_[0] = 0; indices_[1] = 1; indices_[2] = 2;
	indices_[3] = 0; indices_[4] = 3; indices_[5] = 2;
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

	//IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indices_));
	InitializeIndexData();
}

//マテリアルデータの初期化
void SpriteRenderer::InitializeMaterialData(){
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void SpriteRenderer::CreateMaterialResource(){
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(MaterialForSprite));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//トランスフォーメーション行列リソースの生成
void SpriteRenderer::CreateTransformationMatrixResource(){
	//座標変換行列リソースを作成する
	transformationMatrixResource_ = directXBase_->CreateBufferResource(sizeof(TransformationMatrixForSprite));
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrix_));
	//単位行列を書き込んでおく
	transformationMatrix_->wvp = Matrix4x4::Identity4x4();
	transformationMatrix_->world = Matrix4x4::Identity4x4();
}
