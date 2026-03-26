#include "Shape.h"
#include "TextureManager.h"
#include "DirectXBase.h"
#include "algorithm/Rendering.h"
#include "algorithm/Math.h"
#include "GraphicsPipeline.h"
#include "Blend.h"
#include "Camera.h"
#include "ImGuiManager.h"

//コンストラクタ
Shape::Shape() {}

//デストラクタ
Shape::~Shape() {}

//初期化
void Shape::Initialize(DirectXBase* directXBase, TextureManager* textureManager, Camera* camera, const std::string& textureName) {
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;
	//テクスチャの管理を記録
	textureManager_ = textureManager;
	//ブレンド
	blend_ = std::make_unique<Blend>();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Shape.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Shape.PS.hlsl");
	//デプスステンシルステート
	directXBase_->InitializeDepthStencilForObject3d();
	makeGraphicsPipeline_->SetDirectXBase(directXBase);
	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForSprite();
	//ルートシグネイチャの保存
	makeGraphicsPipeline_->CreateRootSignature();
	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerSatate();
	//頂点シェーダBlob
	makeGraphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	makeGraphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++) {
		//ブレンドステート
		makeGraphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		graphicsPipelineStates_[i] = makeGraphicsPipeline_->CreateGraphicsPipeline(directXBase_->GetDepthStencil());
	}
	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();
	//頂点データの生成
	CreateVertexResource();
	//インデックスリソースの生成
	CreateIndexResource();
	//マテリアルデータの生成
	CreateMaterialResource();
	//テクスチャのファイルパスの記録
	modelData_.material.textureFilePath = "engine/resources/textures/" + textureName;
	//テクスチャの読み込み
	textureManager_->LoadTexture(modelData_.material.textureFilePath);
	//uvTransform変数を作る
	uvTransform_ = { {1.0f,1.0f},0.0f,{0.0f,0.0f} };
	transform_.Initialize();
	camera_ = camera;
	//wvpリソースの初期化
	CreateTransformationMatrixResource();
}

//更新
void Shape::Update() {
	//ワールドトランスフォームの更新
	UpdateTransform();
	//UVTransform
	materialData_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
}

//デバッグ
void Shape::Debug() {
	ImGuiManager::DragTransform(transform_);
}

//描画
void Shape::Draw() {
	//2Dオブジェクトの共通部分
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//PSOの設定
	auto pso = graphicsPipelineStates_[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//座標変換行列CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());//wvp
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//SRVのDescriptorTableの先頭を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(modelData_.material.textureFilePath));
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

//頂点データの初期化
void Shape::InitializeVertexData() {
	modelData_.vertices.clear();

	// 左上
	modelData_.vertices.push_back({
		.position = {-1.0f, 1.0f, 0.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {0.0f, 0.0f, 1.0f}
	});

	// 右上
	modelData_.vertices.push_back({
		.position = {1.0f, 1.0f, 0.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {0.0f, 0.0f, 1.0f}
	});

	// 左下
	modelData_.vertices.push_back({
		.position = {-1.0f, -1.0f, 0.0f, 1.0f},
		.texcoord = {0.0f, 1.0f},
		.normal = {0.0f, 0.0f, 1.0f}
	});

	// 右下
	modelData_.vertices.push_back({
		.position = {1.0f, -1.0f, 0.0f, 1.0f},
		.texcoord = {1.0f, 1.0f},
		.normal = {0.0f, 0.0f, 1.0f}
	});
}

//インデックスリソースの生成
void Shape::CreateIndexResource() {
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * 6);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 2;
	indexData_[4] = 1;
	indexData_[5] = 3;
}

//頂点データの生成
void Shape::CreateVertexResource() {
	//頂点データの初期化
	InitializeVertexData();
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());//頂点データをリソースにコピー
}

//マテリアルデータの初期化
void Shape::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void Shape::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//WorldTransformation行列リソースの生成
void Shape::CreateTransformationMatrixResource() {
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

//座標の更新
void Shape::UpdateTransform() {
	worldMatrix_ = Rendering::MakeAffineMatrix(transform_);
	//TransformからWorldMatrixを作る
	//if (parent_) {
	//	worldMatrix_ = worldMatrix_ * parent_->worldMatrix_;
	//}
	//wvpの書き込み
	if (camera_) {
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		wvpData_->wvp = worldMatrix_ * viewProjectionMatrix;
	} else {
		wvpData_->wvp = worldMatrix_;
	}
	//ワールド行列を送信
	wvpData_->world = worldMatrix_;
	//逆行列の転置行列を送信
	wvpData_->worldInverseTranspose = worldMatrix_.InverseTranspose();
}
