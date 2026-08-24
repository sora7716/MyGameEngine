#include "SkyBoxRenderer.h"
#include "Camera.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include <cassert>
#include <cstring>

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

	//頂点リソースの生成
	CreateVertexResource();

	//インデックスリソースの生成
	CreateIndexResource();

	//マテリアルのリソースの生成
	CreateMaterialResource();

	//ワールド行列のリソースの生成
	CreateWorldMatrixResource();
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
void SkyBoxRenderer::Draw(uint32_t instanceIndex, Camera* renderCamera){
	//スカイボックスの描画データ
	SkyBoxRenderData skyBoxRenderData = skyBoxRenderDatas_[instanceIndex];

	//存在していなかったら
	if (!skyBoxRenderData.isActive){
		return;
	}

	//描画データを反映
	//マテリアル
	*materialData_ = skyBoxRenderData.material;
	//ワールド行列
	*worldMatrix_ = skyBoxRenderData.worldMatrix;

	//カメラ
	renderCamera->DrawSetting(3);
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldMatrixResource_->GetGPUVirtualAddress());
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//IndexBufferViewを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(skyBoxRenderData.imageFileName));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(kIndexCount, 1, 0, 0, 0);
}

//ブレンドモードの取得
BlendMode SkyBoxRenderer::GetBlendMode(uint32_t instanceIndex){
	return skyBoxRenderDatas_[instanceIndex].blendMode;
}

//描画データのサイズの取得
uint32_t SkyBoxRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(skyBoxRenderDatas_.size());
}

//頂点データの初期化
void SkyBoxRenderer::InitializeVertexData(){
	//サイズを設定
	vertices_.resize(kVertexCount);
	//右面
	//位置
	vertices_[0].vertex = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[1].vertex = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[2].vertex = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[3].vertex = { 1.0f,-1.0f,-1.0f,1.0f };
	//Texcoord
	vertices_[0].texcoord = { 1.0f,1.0f,1.0f };
	vertices_[1].texcoord = { 1.0f,1.0f,-1.0f };
	vertices_[2].texcoord = { 1.0f,-1.0f,1.0f };
	vertices_[3].texcoord = { 1.0f,-1.0f,-1.0f };

	//左面
	vertices_[4].vertex = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[5].vertex = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[6].vertex = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[7].vertex = { -1.0f,-1.0f,1.0f,1.0f };
	//Texcoord
	vertices_[4].texcoord = { -1.0f,1.0f,-1.0f };
	vertices_[5].texcoord = { -1.0f,1.0f,1.0f };
	vertices_[6].texcoord = { -1.0f,-1.0f,-1.0f };
	vertices_[7].texcoord = { -1.0f,-1.0f,1.0f };

	//前面
	vertices_[8].vertex = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[9].vertex = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[10].vertex = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[11].vertex = { 1.0f,-1.0f,1.0f,1.0f };
	//Texcoord
	vertices_[8].texcoord = { -1.0f,1.0f,1.0f };
	vertices_[9].texcoord = { 1.0f,1.0f,1.0f };
	vertices_[10].texcoord = { -1.0f,-1.0f,1.0f };
	vertices_[11].texcoord = { 1.0f,-1.0f,1.0f };

	//後面
	vertices_[12].vertex = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[13].vertex = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[14].vertex = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[15].vertex = { 1.0f,-1.0f,-1.0f,1.0f };
	//Texcoord
	vertices_[12].texcoord = { -1.0f,1.0f,-1.0f };
	vertices_[13].texcoord = { 1.0f,1.0f,-1.0f };
	vertices_[14].texcoord = { -1.0f,-1.0f,-1.0f };
	vertices_[15].texcoord = { 1.0f,-1.0f,-1.0f };

	//上面
	vertices_[16].vertex = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[17].vertex = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[18].vertex = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[19].vertex = { -1.0f,1.0f,1.0f,1.0f };
	//Texcoord
	vertices_[16].texcoord = { -1.0f,1.0f,-1.0f };
	vertices_[17].texcoord = { 1.0f,1.0f,-1.0f };
	vertices_[18].texcoord = { 1.0f,1.0f,1.0f };
	vertices_[19].texcoord = { -1.0f,1.0f,1.0f };

	//下面
	vertices_[20].vertex = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[21].vertex = { 1.0f,-1.0f,-1.0f,1.0f };
	vertices_[22].vertex = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[23].vertex = { -1.0f,-1.0f,1.0f,1.0f };
	//Texcoord
	vertices_[20].texcoord = { -1.0f,-1.0f,-1.0f };
	vertices_[21].texcoord = { 1.0f,-1.0f,-1.0f };
	vertices_[22].texcoord = { 1.0f,-1.0f,1.0f };
	vertices_[23].texcoord = { -1.0f,-1.0f,1.0f };

}

//頂点データの生成
void SkyBoxRenderer::CreateVertexResource(){
	//頂点データの初期化
	InitializeVertexData();

	//VertexResourceを作成する
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(SkyBoxVertexData) * kVertexCount);
	//VertexBufferViewを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(SkyBoxVertexData) * kVertexCount;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(SkyBoxVertexData);

	//VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	SkyBoxVertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点データをリソースにコピー
	std::memcpy(vertexData, vertices_.data(), sizeof(SkyBoxVertexData) * kVertexCount);
}

//インデックスデータの初期化
void SkyBoxRenderer::InitializeIndexData(){
	//サイズを設定
	indices_.resize(kIndexCount);
	//右面
	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;
	indices_[3] = 2;
	indices_[4] = 1;
	indices_[5] = 3;
	//左面
	indices_[6] = 4;
	indices_[7] = 5;
	indices_[8] = 6;
	indices_[9] = 6;
	indices_[10] = 5;
	indices_[11] = 7;
	//前面
	indices_[12] = 8;
	indices_[13] = 9;
	indices_[14] = 10;
	indices_[15] = 10;
	indices_[16] = 9;
	indices_[17] = 11;
	//後面
	indices_[18] = 12;
	indices_[19] = 14;
	indices_[20] = 13;
	indices_[21] = 13;
	indices_[22] = 14;
	indices_[23] = 15;
	//上面
	indices_[24] = 16;
	indices_[25] = 17;
	indices_[26] = 18;
	indices_[27] = 18;
	indices_[28] = 19;
	indices_[29] = 16;
	//下面
	indices_[30] = 21;
	indices_[31] = 23;
	indices_[32] = 22;
	indices_[33] = 21;
	indices_[34] = 20;
	indices_[35] = 23;
}

//インデックスリソースの生成
void SkyBoxRenderer::CreateIndexResource(){
	//インデックスデータの初期化
	InitializeIndexData();

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
	uint32_t* indexData = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	//インデックスデータをリソースにコピー
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * kIndexCount);
}

//マテリアルリソースの生成
void SkyBoxRenderer::CreateMaterialResource(){
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Vector4));
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//色を書き込む
	*materialData_ = Vector4::MakeWhiteColor();
}

//座標変換行列リソースの生成
void SkyBoxRenderer::CreateWorldMatrixResource(){
	//座標変換行列リソースを作成する
	worldMatrixResource_ = directXBase_->CreateBufferResource(sizeof(Matrix4x4));
	//書き込むためのアドレス
	worldMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&worldMatrix_));
	//単位行列を書き込んでおく
	*worldMatrix_ = Matrix4x4::Identity4x4();
}