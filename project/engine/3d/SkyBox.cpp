#include "SkyBox.h"
#include "MathUtility.h"
#include "TextureManager.h"
#include "DirectXBase.h"
#include "WinApi.h"
#include "ImGuiManager.h"
#include "MatrixUtility.h"
#include "TextureManager.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Blend.h"
#include <cassert>

//コンストラクタ
SkyBox::SkyBox() {
}

//デストラクタ
SkyBox::~SkyBox() {
}

//初期化
void SkyBox::Initialize(DirectXBase* directXBase, TextureManager* textureManager, const std::string& imageFileName, Camera* camera) {
	//DirectXの基盤を受け取る
	directXBase_ = directXBase;
	//テクスチャマネージャー
	textureManager_ = textureManager;
	//カメラの作成
	camera_ = camera;
	//ブレンド
	blend_ = std::make_unique<Blend>();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"SkyBox.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"SkyBox.PS.hlsl");
	//DirectXBaseの記録
	makeGraphicsPipeline_->SetDirectXBase(directXBase);
	//深度バッファ
	makeGraphicsPipeline_->CreateDepthStencilResourceForParticle();
	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForSprite();
	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDescForSkyBox();
	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerState();
	//頂点シェーダBlob
	makeGraphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	makeGraphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++) {
		//ブレンドステート
		makeGraphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		graphicsPipelineStates_[i] = makeGraphicsPipeline_->CreateGraphicsPipeline();
	}
	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();
	//頂点データの生成
	CreateVertexResource();
	//インデックスデータの生成
	CreateIndexResource();
	//マテリアルデータの生成
	CreateMaterialResource();
	//スプライトファイルパスを記録
	imageFileName_ = "engine/resources/textures/" + imageFileName;
	//スプライトの共通部分
	textureManager->LoadTexture(imageFileName_);
	//ワールド座標
	transform_ = { {10.0f,10.0f,20.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//wvpリソースの初期化
	CreateTransformationMatrixResource();
}

//更新
void SkyBox::Update() {
	//TransformからWorldMatrixを作る
	wvpData_->world = MatrixUtility::MakeAffineMatrix(transform_);
	//wvpの書き込み
	const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
	wvpData_->wvp = wvpData_->world * viewProjectionMatrix;
}

void SkyBox::Debug() {
	ImGuiManager::DragTransform(transform_);
}

//描画処理
void SkyBox::Draw() {
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//PSOの設定
	auto pso = graphicsPipelineStates_[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//IndexBufferViewを設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(imageFileName_));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(kIndexCount, 1, 0, 0, 0);
}

// UVの座標変換の更新
void SkyBox::UpdateUVTransform(Transform2d uvTransform) {
	//UVTransform
	materialData_->uvMatrix = MatrixUtility::MakeUVAffineMatrix(uvTransform);
}

//頂点データの初期化
void SkyBox::InitializeVertexData() {
	//サイズを設定
	skyBoxProp.resize(kVertexCount);
	//右面
	//位置
	skyBoxProp[0].vertexPos = { 1.0f,1.0f,1.0f,1.0f };
	skyBoxProp[1].vertexPos = { 1.0f,1.0f,-1.0f,1.0f };
	skyBoxProp[2].vertexPos = { 1.0f,-1.0f,1.0f,1.0f };
	skyBoxProp[3].vertexPos = { 1.0f,-1.0f,-1.0f,1.0f };
	//Texcoord
	skyBoxProp[0].texcoord = { 1.0f,1.0f,1.0f };
	skyBoxProp[1].texcoord = { 1.0f,1.0f,-1.0f };
	skyBoxProp[2].texcoord = { 1.0f,-1.0f,1.0f};
	skyBoxProp[3].texcoord = { 1.0f,-1.0f,-1.0f };

	//左面
	skyBoxProp[4].vertexPos = { -1.0f,1.0f,-1.0f,1.0f };
	skyBoxProp[5].vertexPos = { -1.0f,1.0f,1.0f,1.0f };
	skyBoxProp[6].vertexPos = { -1.0f,-1.0f,-1.0f,1.0f };
	skyBoxProp[7].vertexPos = { -1.0f,-1.0f,1.0f,1.0f };
	//Texcoord
	skyBoxProp[4].texcoord = { -1.0f,1.0f,-1.0f };
	skyBoxProp[5].texcoord = { -1.0f,1.0f,1.0f };
	skyBoxProp[6].texcoord = { -1.0f,-1.0f,-1.0f };
	skyBoxProp[7].texcoord = { -1.0f,-1.0f,1.0f };

	//前面
	skyBoxProp[8].vertexPos = { -1.0f,1.0f,1.0f,1.0f };
	skyBoxProp[9].vertexPos = { 1.0f,1.0f,1.0f,1.0f };
	skyBoxProp[10].vertexPos = { -1.0f,-1.0f,1.0f,1.0f };
	skyBoxProp[11].vertexPos = { 1.0f,-1.0f,1.0f,1.0f };
	//Texcoord
	skyBoxProp[8].texcoord = { -1.0f,1.0f,1.0f };
	skyBoxProp[9].texcoord = { 1.0f,1.0f,1.0f };
	skyBoxProp[10].texcoord = { -1.0f,-1.0f,1.0f };
	skyBoxProp[11].texcoord = { 1.0f,-1.0f,1.0f };

	//後面
	skyBoxProp[12].vertexPos = { -1.0f,1.0f,-1.0f,1.0f };
	skyBoxProp[13].vertexPos = { 1.0f,1.0f,-1.0f,1.0f };
	skyBoxProp[14].vertexPos = { -1.0f,-1.0f,-1.0f,1.0f };
	skyBoxProp[15].vertexPos = { 1.0f,-1.0f,-1.0f,1.0f };
	//Texcoord
	skyBoxProp[12].texcoord = { -1.0f,1.0f,-1.0f };
	skyBoxProp[13].texcoord = { 1.0f,1.0f,-1.0f };
	skyBoxProp[14].texcoord = { -1.0f,-1.0f,-1.0f };
	skyBoxProp[15].texcoord = { 1.0f,-1.0f,-1.0f };

	//上面
	skyBoxProp[16].vertexPos = { -1.0f,1.0f,-1.0f,1.0f };
	skyBoxProp[17].vertexPos = { 1.0f,1.0f,-1.0f,1.0f };
	skyBoxProp[18].vertexPos = { 1.0f,1.0f,1.0f,1.0f };
	skyBoxProp[19].vertexPos = { -1.0f,1.0f,1.0f,1.0f };
	//Texcoord
	skyBoxProp[16].texcoord = { -1.0f,1.0f,-1.0f };
	skyBoxProp[17].texcoord = { 1.0f,1.0f,-1.0f };
	skyBoxProp[18].texcoord = { 1.0f,1.0f,1.0f };
	skyBoxProp[19].texcoord = { -1.0f,1.0f,1.0f };

	//下面
	skyBoxProp[20].vertexPos = { -1.0f,-1.0f,-1.0f,1.0f };
	skyBoxProp[21].vertexPos = { 1.0f,-1.0f,-1.0f,1.0f };
	skyBoxProp[22].vertexPos = { 1.0f,-1.0f,1.0f,1.0f };
	skyBoxProp[23].vertexPos = { -1.0f,-1.0f,1.0f,1.0f };
	//Texcoord
	skyBoxProp[20].texcoord = { -1.0f,-1.0f,-1.0f };
	skyBoxProp[21].texcoord = { 1.0f,-1.0f,-1.0f };
	skyBoxProp[22].texcoord = { 1.0f,-1.0f,1.0f };
	skyBoxProp[23].texcoord = { -1.0f,-1.0f,1.0f };

}

//頂点データの生成
void SkyBox::CreateVertexResource() {
	//頂点データの初期化
	InitializeVertexData();
	//VertexResourceを作成する
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(SkyBoxProp) * kVertexCount);
	//VertexBufferViewを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(SkyBoxProp) * kVertexCount;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(SkyBoxProp);

	//VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	SkyBoxProp* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//頂点データをリソースにコピー
	std::memcpy(vertexData, skyBoxProp.data(), sizeof(SkyBoxProp) * kVertexCount);
}

//インデックスデータの初期化
void SkyBox::InitializeIndexData() {
	//サイズを設定
	index_.resize(kIndexCount);
	//右面
	index_[0] = 0;
	index_[1] = 1;
	index_[2] = 2;
	index_[3] = 2;
	index_[4] = 1;
	index_[5] = 3;
	//左面
	index_[6] = 4;
	index_[7] = 5;
	index_[8] = 6;
	index_[9] = 6;
	index_[10] = 5;
	index_[11] = 7;
	//前面
	index_[12] = 8;
	index_[13] = 9;
	index_[14] = 10;
	index_[15] = 10;
	index_[16] = 9;
	index_[17] = 11;
	//後面
	index_[18] = 12;
	index_[19] = 14;
	index_[20] = 13;
	index_[21] = 13;
	index_[22] = 14;
	index_[23] = 15;
	//上面
	index_[24] = 16;
	index_[25] = 17;
	index_[26] = 18;
	index_[27] = 18;
	index_[28] = 19;
	index_[29] = 16;
	//下面
	index_[30] = 21;
	index_[31] = 23;
	index_[32] = 22;
	index_[33] = 21;
	index_[34] = 20;
	index_[35] = 23;
}

//インデックスリソースの生成
void SkyBox::CreateIndexResource() {
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
	std::memcpy(indexData, index_.data(), sizeof(uint32_t) * kIndexCount);
}

//マテリアルデータの初期化
void SkyBox::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void SkyBox::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//座標変換行列リソースの生成
void SkyBox::CreateTransformationMatrixResource() {
	//座標変換行列リソースを作成する
	wvpResource_ = directXBase_->CreateBufferResource(sizeof(TransformationMatrix));
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	wvpData_->wvp = Matrix4x4::Identity4x4();
	wvpData_->world = Matrix4x4::Identity4x4();
	wvpData_->worldInverseTranspose = Matrix4x4::Identity4x4();
}
