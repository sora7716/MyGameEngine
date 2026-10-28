#include "RootSignatureBuilder.h"
#include "Logger.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;
//コンストラクタ
RootSignatureBuilder::RootSignatureBuilder(){
}

//デストラクタ
RootSignatureBuilder::~RootSignatureBuilder(){
}

//初期化
void RootSignatureBuilder::Initialize(){
	//RootSignature作成
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
}

//構築
ComPtr<ID3D12RootSignature> RootSignatureBuilder::Build(ID3D12Device* device){
	//Tableを確認
	uint32_t rangeIndex = 0;
	for (D3D12_ROOT_PARAMETER& rootParameter : rootParameters_){
		//デスクリプタテーブルがあった場合
		if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE){
			rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges_[rangeIndex];
			rangeIndex++;
		}
	}

	//サンプラーの設定
	descriptionRootSignature_.pStaticSamplers = staticSamplers_.data();
	descriptionRootSignature_.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());

	//デスクリプタルートシグネイチャを設定
	descriptionRootSignature_.pParameters = rootParameters_.data();//ルートパラメータ配列へのポインタ
	descriptionRootSignature_.NumParameters = static_cast<UINT>(rootParameters_.size());//配列の長さ

	//ルートシグネイチャ
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//ルートシグネイチャBlob
	ComPtr<ID3DBlob>signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	//シリアライズしてバイナリにする
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)){
		if (errorBlob){
			Logger::OutputLog(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		}
		assert(false);
		return nullptr;
	}

	HRESULT result = S_FALSE;
	result = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	return rootSignature_;
}

//コンスタントバッファビューの追加
void RootSignatureBuilder::AddCBV(ShaderType shaderType, uint32_t registerNumber){
	//RootParameterの作成
	D3D12_ROOT_PARAMETER rootParameter = {};
	//色情報
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderType);
	rootParameter.Descriptor.ShaderRegister = static_cast<UINT>(registerNumber);

	//配列に追加
	rootParameters_.push_back(rootParameter);
}

//ストラクチャバッファテーブルの追加
void RootSignatureBuilder::AddSRVTable(ShaderType shaderType, uint32_t registerNumber){
	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange = {};
	descriptorRange.BaseShaderRegister = static_cast<UINT>(registerNumber);//0から始まる
	descriptorRange.NumDescriptors = 1;//数は1つ
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	//descriptorRangeの配列に追加
	descriptorRanges_.push_back(descriptorRange);

	//RootParameterの作成
	D3D12_ROOT_PARAMETER rootParameter = {};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//SRVを使う
	rootParameter.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderType);
	rootParameter.DescriptorTable.NumDescriptorRanges = 1;

	//配列に追加
	rootParameters_.push_back(rootParameter);
}

//サンプラーの追加
void RootSignatureBuilder::AddStaticSampler(ShaderType shaderType, uint32_t registerNumber){
	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSampler = {};
	staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイナリフィルター
	staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler.MaxLOD = D3D12_FLOAT32_MAX;//ありたっけのMipmapを使う
	staticSampler.ShaderRegister = static_cast<UINT>(registerNumber);//レジスタ番号
	staticSampler.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderType);//使うシェーダを設定

	staticSamplers_.push_back(staticSampler);
}
