#include "Shape.h"
#include "TextureManager.h"
#include "DirectXBase.h"
#include "algorithm/Rendering.h"
#include "algorithm/Math.h"
#include "Log.h"
#include "GraphicsPipeline.h"
#include "Camera.h"
#include "ImGuiManager.h"
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")

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
	//カメラの記録
	camera_ = camera;

	//グラフィックスパイプラインの作成
	BuildGraphicsPipeline();

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
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
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

//ルートシグネイチャBlobの生成
void Shape::CreateRootSignatureBlob() {
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイナリフィルター
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありたっけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算


	//RootParameterの作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//色情報
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使うb0のbと一致する	
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインドb0の0と一致する

	//Transform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号

	//DescriptorTable(DescriptorRangeをまとめたもの)
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ
	ComPtr<ID3DBlob> errorBlob = nullptr;
	//シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob);
	if (FAILED(hr)) {
		Log::ConsolePrintf(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
}

//ルートシグネイチャの生成
void Shape::CreateRootSignature() {
	HRESULT result = S_FALSE;
	result = directXBase_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
}

//インプットレイアウトの初期化
void Shape::InitializeInputLayoutDesc() {
	//InputElementDesc
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//InputLayout
	inputLayoutDesc_.pInputElementDescs = inputElementDescs;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs);
}

//ラスタライザステートの初期化
void Shape::InitializeRasterizerSatate() {
	//裏面(時計周り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

//頂点シェーダのコンパイル
void Shape::CompileVertexShader() {
	//VertexShader
	vertexShaderBlob_ = directXBase_->CompilerShader(L"engine/resources/shaders/" + vertexShaderFileName_, L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);
}

//ピクセルシェーダのコンパイル
void Shape::CompilePixelShader() {
	//PixelShader
	pixelShaderBlob_ = directXBase_->CompilerShader(L"engine/resources/shaders/" + pixelShaderFileName_, L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

//ブレンドステートの初期化
void Shape::InitializeBlendState() {
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

//PSOの生成
ComPtr<ID3D12PipelineState> Shape::CreateGraphicsPipeline() {
	HRESULT result = S_FALSE;
	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	//InputLayout
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;
	//VertexShader
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	//PixelShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	//BlendState
	graphicsPipelineStateDesc.BlendState = blendDesc_;
	//RasterizerState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定(気にしなくてよい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = directXBase_->GetDepthStencil();
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	ComPtr<ID3D12PipelineState>graphicsPipelineState;
	result = directXBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(result));
	return graphicsPipelineState;
}

//グラフィックスパイプラインの構築
void Shape::BuildGraphicsPipeline() {
	//デプスステンシルステート
	directXBase_->InitializeDepthStencilForObject3d();
	//ルートシグネイチャBlobの生成
	CreateRootSignatureBlob();
	//ルートシグネイチャの保存
	CreateRootSignature();
	//インプットレイアウト
	InitializeInputLayoutDesc();
	//ラスタライザステート
	InitializeRasterizerSatate();
	//頂点シェーダBlob
	CompileVertexShader();
	//ピクセルシェーダBlob
	CompilePixelShader();
	//PSO
		//ブレンドステート
	InitializeBlendState();
	//グラフィックスパイプラインの生成
	graphicsPipelineState_ = CreateGraphicsPipeline();
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
