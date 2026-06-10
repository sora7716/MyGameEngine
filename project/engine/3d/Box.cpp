#include "Box.h"
#include "DirectXBase.h"
#include "algorithms/Rendering.h"
#include "algorithms/Math.h"
#include "TextureManager.h"
#include "Logger.h"
#include "Camera.h"
#include "ImGuiManager.h"
using namespace Microsoft::WRL;

#pragma comment(lib,"d3d12.lib")

//コンストラクタ
Box::Box() {}

//デストラクタ
Box::~Box() {}

//初期化
void Box::Initialize(DirectXBase* directXBase, TextureManager* textureManager, Camera* camera) {
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;
	//カメラの記録
	camera_ = camera;
	//テクスチャの管理の記録
	textureManager_ = textureManager;

	textureManager_->LoadTexture(texturePath_);

	//グラフィックスパイプラインの作成
	BuildGraphicsPipeline();

	//頂点データの生成
	CreateVertexResource();
	//インデックスリソースの生成
	CreateIndexResource();
	//マテリアルリソースの生成
	CreateMaterialResource();
	//平行光源リソースの生成
	CreateDirectionLight();
	//カメラリソースの生成
	CreateCameraResource(camera_->GetTranslate());
	
	//ワールド座標の初期化
	transform_.Initialize();

	//wvpリソースの初期化
	CreateTransformationMatrixResource();
	
	//UV座標の初期化
	uvTransform_.Initialize();
}

//更新
void Box::Update() {
	//頂点データの設定
	//SettingVertexData();
	//ワールド座標の更新
	UpdateTransform();
	//UV座標の更新
	UpdateUvTransform();
}

//デバッグ
void Box::Debug() {
#ifdef USE_IMGUI
	ImGuiManager::DragTransform(transform_);
	ImGui::ColorEdit4("materialColor", &material_->color.x);
	ImGui::DragFloat2("uvScale", &uvTransform_.scale.x, 0.1f);
	ImGui::DragFloat("uvRotate", &uvTransform_.rotate, 0.1f);
	ImGui::DragFloat2("uvTranslate", &uvTransform_.translate.x, 0.1f);
	if (ImGuiManager::CheckBoxToInt("enableLighting", material_->enableLighting)) {
		ImGui::DragFloat("shiness", &material_->shininess, 0.1f);
		ImGui::ColorEdit4("lightColor", &directionalLightPtr_->color.x);
		ImGui::DragFloat3("direction", &directionalLightPtr_->direction.x);
		ImGui::DragFloat("intensity", &directionalLightPtr_->intensity);
		ImGuiManager::CheckBoxToInt("isLambert", directionalLightPtr_->isLambert);
		ImGuiManager::CheckBoxToInt("isBlinnPhong", directionalLightPtr_->isBlinnPhong);
	}

#endif // USE_IMGUI
}

//描画
void Box::Draw() {
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
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定(テクスチャ)
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(texturePath_));
	//平行光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//カメラCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	//描画(DrawCall/ドローコール)
	directXBase_->GetCommandList()->DrawIndexedInstanced(kIndexCount, 1, 0, 0, 0);
}

//カメラのセッター
void Box::SetCamera(Camera* camera) {
	camera_ = camera;
}

//頂点データの設定
void Box::SettingVertexData() {
	//vertex

	//Z+
	// 左上
	vertexData_[0] = {
		.position = {-1.0f, 1.0f, 1.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {0.0f, 0.0f, 1.0f}
	};

	//右上
	vertexData_[1] = {
		.position = {1.0f, 1.0f, 1.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {0.0f, 0.0f, 1.0f}
	};

	//左下
	vertexData_[2] = {
			.position = {-1.0f, -1.0f, 1.0f, 1.0f},
			.texcoord = {0.0f, 1.0f},
			.normal = {0.0f, 0.0f, 1.0f}
	};

	//右下
	vertexData_[3] = {
			.position = {1.0f, -1.0f, 1.0f, 1.0f},
			.texcoord = {1.0f, 1.0f},
			.normal = {0.0f, 0.0f, 1.0f}
	};

	//Z-
	// 左上
	vertexData_[4] = {
		.position = {-1.0f, 1.0f, -1.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {0.0f, 0.0f, -1.0f}
	};

	//右上
	vertexData_[5] = {
		.position = {1.0f, 1.0f, -1.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {0.0f, 0.0f, -1.0f}
	};

	//左下
	vertexData_[6] = {
			.position = {-1.0f, -1.0f, -1.0f, 1.0f},
			.texcoord = {0.0f, 1.0f},
			.normal = {0.0f, 0.0f, -1.0f}
	};

	//右下
	vertexData_[7] = {
			.position = {1.0f, -1.0f, -1.0f, 1.0f},
			.texcoord = {1.0f, 1.0f},
			.normal = {0.0f, 0.0f, -1.0f}
	};

	//X+
	// 左上
	vertexData_[8] = {
		.position = {1.0f, -1.0f, 1.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {1.0f, 0.0f, 0.0f}
	};

	//右上
	vertexData_[9] = {
		.position = {1.0f, 1.0f, 1.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {1.0f, 0.0f, 0.0f}
	};

	//左下
	vertexData_[10] = {
			.position = {1.0f, -1.0f, -1.0f, 1.0f},
			.texcoord = {0.0f, 1.0f},
			.normal = {1.0f, 0.0f, 0.0f}
	};

	//右下
	vertexData_[11] = {
			.position = {1.0f, 1.0f, -1.0f, 1.0f},
			.texcoord = {1.0f, 1.0f},
			.normal = {1.0f, 0.0f, 0.0f}
	};

	//X-
	// 左上
	vertexData_[12] = {
		.position = {-1.0f, -1.0f, 1.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {-1.0f, 0.0f, 0.0f}
	};

	//右上
	vertexData_[13] = {
		.position = {-1.0f, 1.0f, 1.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {-1.0f, 0.0f, 0.0f}
	};

	//左下
	vertexData_[14] = {
			.position = {-1.0f, -1.0f, -1.0f, 1.0f},
			.texcoord = {0.0f, 1.0f},
			.normal = {-1.0f, 0.0f, 0.0f}
	};

	//右下
	vertexData_[15] = {
			.position = {-1.0f, 1.0f, -1.0f, 1.0f},
			.texcoord = {1.0f, 1.0f},
			.normal = {-1.0f, 0.0f, 0.0f}
	};

	//Y+
	// 左上
	vertexData_[16] = {
		.position = {-1.0f, 1.0f, 1.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {0.0f, 1.0f, 0.0f}
	};

	//右上
	vertexData_[17] = {
		.position = {1.0f, 1.0f, 1.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {0.0f, 1.0f, 0.0f}
	};

	//左下
	vertexData_[18] = {
			.position = {-1.0f, 1.0f, -1.0f, 1.0f},
			.texcoord = {0.0f, 1.0f},
			.normal = {0.0f, 1.0f, 0.0f}
	};

	//右下
	vertexData_[19] = {
			.position = {1.0f, 1.0f, -1.0f, 1.0f},
			.texcoord = {1.0f, 1.0f},
			.normal = {0.0f, 1.0f, 0.0f}
	};

	//Y-
	// 左上
	vertexData_[20] = {
		.position = {-1.0f, -1.0f, 1.0f, 1.0f},
		.texcoord = {0.0f, 0.0f},
		.normal = {0.0f, -1.0f, 0.0f}
	};

	//右上
	vertexData_[21] = {
		.position = {1.0f, -1.0f, 1.0f, 1.0f},
		.texcoord = {1.0f, 0.0f},
		.normal = {0.0f, -1.0f, 0.0f}
	};

	//左下
	vertexData_[22] = {
			.position = {-1.0f, -1.0f, -1.0f, 1.0f},
			.texcoord = {0.0f, 1.0f},
			.normal = {0.0f, -1.0f, 0.0f}
	};

	//右下
	vertexData_[23] = {
			.position = {1.0f, -1.0f, -1.0f, 1.0f},
			.texcoord = {1.0f, 1.0f},
			.normal = {0.0f, -1.0f, 0.0f}
	};
}

//インデックスデータの設定
void Box::SettingIndexData() {
	// Z+  正面
	indexData_[0] = 0;
	indexData_[1] = 2;
	indexData_[2] = 1;
	indexData_[3] = 2;
	indexData_[4] = 3;
	indexData_[5] = 1;

	// Z-  背面
	indexData_[6] = 4;
	indexData_[7] = 5;
	indexData_[8] = 6;
	indexData_[9] = 6;
	indexData_[10] = 5;
	indexData_[11] = 7;

	// X+  右面
	indexData_[12] = 8;
	indexData_[13] = 10;
	indexData_[14] = 9;
	indexData_[15] = 10;
	indexData_[16] = 11;
	indexData_[17] = 9;

	// X-  左面
	indexData_[18] = 12;
	indexData_[19] = 13;
	indexData_[20] = 14;
	indexData_[21] = 14;
	indexData_[22] = 13;
	indexData_[23] = 15;

	// Y+  上面
	indexData_[24] = 16;
	indexData_[25] = 17;
	indexData_[26] = 18;
	indexData_[27] = 18;
	indexData_[28] = 17;
	indexData_[29] = 19;

	// Y-  下面
	indexData_[30] = 20;
	indexData_[31] = 22;
	indexData_[32] = 21;
	indexData_[33] = 22;
	indexData_[34] = 23;
	indexData_[35] = 21;
}

//インデックスリソースの生成
void Box::CreateIndexResource() {
	indexResource_ = directXBase_->CreateBufferResource(sizeof(uint32_t) * kIndexCount);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * kIndexCount);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	//インデックスデータの初期化
	SettingIndexData();
}

//頂点データの生成
void Box::CreateVertexResource() {
	//頂点リソースを生成
	vertexResource_ = directXBase_->CreateBufferResource(sizeof(VertexData) * kVertexCount);
	//VertexBufferViewを作成する(頂点バッファービュー)
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * kVertexCount);
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));//書き込むためのアドレスを取得

	//頂点データの初期化
	SettingVertexData();
}

//マテリアルデータの初期化
void Box::InitializeMaterialData() {
	material_->color = Vector4::MakeWhiteColor();
	material_->enableLighting = true;
	material_->shininess = 10.0f;
	material_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void Box::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
}

//WorldTransformation行列リソースの生成
void Box::CreateTransformationMatrixResource() {
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
void Box::CreateRootSignatureBlob() {
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
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;//数は1つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameterの作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	//色情報
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使うb0のbと一致する	
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインドb0の0と一致する

	//Transform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使うb0のbと一致する	
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//PixelShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインドb0の0と一致する

	//DescriptorTable(DescriptorRangeをまとめたもの)
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);//Tableで利用する数

	//平行光源
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う

	//カメラ
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[4].Descriptor.ShaderRegister = 2;//レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ
	ComPtr<ID3DBlob> errorBlob = nullptr;
	//シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob);
	if (FAILED(hr)) {
		Logger::ConsolePrintf(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
}

//ルートシグネイチャの生成
void Box::CreateRootSignature() {
	HRESULT result = S_FALSE;
	result = directXBase_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));
}

//インプットレイアウトの初期化
void Box::InitializeInputLayoutDesc() {
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
void Box::InitializeRasterizerState() {
	//裏面(時計周り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

//頂点シェーダのコンパイル
void Box::CompileVertexShader() {
	//VertexShader
	vertexShaderBlob_ = directXBase_->CompilerShader(L"engine/resources/shaders/" + vertexShaderFileName_, L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);
}

//ピクセルシェーダのコンパイル
void Box::CompilePixelShader() {
	//PixelShader
	pixelShaderBlob_ = directXBase_->CompilerShader(L"engine/resources/shaders/" + pixelShaderFileName_, L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

//ブレンドステートの初期化
void Box::InitializeBlendState() {
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

//PSOの生成
ComPtr<ID3D12PipelineState> Box::CreateGraphicsPipeline() {
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
void Box::BuildGraphicsPipeline() {
	//デプスステンシルステート
	directXBase_->InitializeDepthStencilForObject3d();
	//ルートシグネイチャBlobの生成
	CreateRootSignatureBlob();
	//ルートシグネイチャの保存
	CreateRootSignature();
	//インプットレイアウト
	InitializeInputLayoutDesc();
	//ラスタライザステート
	InitializeRasterizerState();
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
void Box::UpdateTransform() {
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

//UV座標の更新
void Box::UpdateUvTransform() {
	material_->uvMatrix = Rendering::MakeUVAffineMatrix(uvTransform_);
}

//カメラリソースの生成
void Box::CreateCameraResource(const Vector3& cameraTranslate) {
	//光源のリソースを作成
	cameraResource_ = directXBase_->CreateBufferResource(sizeof(CameraForGPU));
	//光源データの書きこみ
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = cameraTranslate;
}

//平行光源の生成
void Box::CreateDirectionLight() {
	//光源のリソースを作成
	directionalLightResource_ = directXBase_->CreateBufferResource(sizeof(DirectionalLight));
	//光源データの書きこみ
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightPtr_));
	directionalLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightPtr_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightPtr_->intensity = 1.0f;
	directionalLightPtr_->isLambert = false;
	directionalLightPtr_->isBlinnPhong = true;
	directionalLightPtr_->enableDirectionalLighting = true;
}