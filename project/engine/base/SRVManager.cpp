#include "SRVManager.h"
#include "DirectXBase.h"
#include <cassert>
using namespace Microsoft::WRL;

//デストラクタ
SRVManager::~SRVManager() {

}

//初期化
void SRVManager::Initialize(DirectXBase* directXBase) {
	//DirectXの基盤部分を記憶する
	directXBase_ = directXBase;
	//デスクリプタヒープの生成
	descriptorHeap_ = directXBase_->MakeDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
	//デスクリプタ1個分のサイズを取得
	descriptorSize_ = directXBase_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

//確保
uint32_t SRVManager::Allocate() {
	//空きリストがあるなら取得
	if (!freeList_.empty()) {
		uint32_t index = freeList_.front();
		freeList_.pop();
		return index;
	}
	//上限チェック
	assert(useIndex_ < kMaxSRVCount);
	//新しい番号を割り当て
	return useIndex_++;
}

//解放
void SRVManager::Free(uint32_t index) {
	//範囲内のインデックスのみ会法
	if (index >= 0 && index < useIndex_) {
		freeList_.push(index);
	}
}

// SRV生成(テクスチャ用)
void SRVManager::CreateSRVForTexture2D(DirectX::TexMetadata metadata, uint32_t srvIndex, ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels) {
	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	} else {
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(mipLevels);
	}
	//設定を元にSRVを生成
	directXBase_->GetDevice()->CreateShaderResourceView(resource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

// SRV生成(Structured Buffer用)
void SRVManager::CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* resource, UINT numElements, UINT structureByteStride) {
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured Buffer の場合は UNKNOWN
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; // バッファ用の設定
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE; // 必要なら RAW を設定
	// 設定を元に SRV を生成
	directXBase_->GetDevice()->CreateShaderResourceView(resource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

//描画開始位置
void SRVManager::PreDraw() {
	//描画用のDescriptorHeapの設定
	ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { descriptorHeap_.Get() };
	directXBase_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());
}

// rootDescriptorTableのセッター
void SRVManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex) {
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, GetGPUDescriptorHandle(srvIndex));
}

// 最大テクスチャを超えて読み込もうとしてるかチェック
bool SRVManager::TextureLimitCheck(uint32_t kSRVTop) {
	return useIndex_ + kSRVTop < kMaxSRVCount;
}

// CPUデスクリプタハンドルのゲッター
D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUDescriptorHandle(uint32_t index) {
	return directXBase_->GetCPUDescriptorHandle(descriptorHeap_, descriptorSize_, index);
}

// GPUデスクリプタハンドルのゲッター
D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUDescriptorHandle(uint32_t index) {
	return directXBase_->GetGPUDescriptorHandle(descriptorHeap_, descriptorSize_, index);
}

// デスクリプタヒープのゲッター
ID3D12DescriptorHeap* SRVManager::GetDescriptorHeap() const {
	return descriptorHeap_.Get();
}

//コンストラクタ
SRVManager::SRVManager(ConstructorKey) {}
