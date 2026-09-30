#include "DSVManager.h"
#include "DirectXBase.h"
#include <cassert>
using namespace Microsoft::WRL;

//生成
std::unique_ptr<DSVManager> DSVManager::Create(ConstructorKey key, DirectXBase* directXBase){
	//生成
	std::unique_ptr<DSVManager>instance = std::make_unique<DSVManager>(key);
	//初期化
	instance->Initialize(directXBase);

	return instance;
}

//コンストラクタ
DSVManager::DSVManager(ConstructorKey){
}

//デストラクタ
DSVManager::~DSVManager(){
}

//初期化
void DSVManager::Initialize(DirectXBase* directXBase){
	//DirectXの基盤部分を記憶する
	directXBase_ = directXBase;
	//デスクリプタヒープの生成
	descriptorHeap_ = directXBase_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxDSVCount, false);
	//デスクリプタ1個分のサイズを取得
	descriptorSize_ = directXBase_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

}

//確保
uint32_t DSVManager::Allocate(){
	//空きリストがあるなら取得
	if (!freeList_.empty()){
		uint32_t index = freeList_.front();
		freeList_.pop();
		return index;
	}
	//上限チェック
	assert(useIndex_ < kMaxDSVCount);
	//新しい番号を割り当て
	return useIndex_++;
}

//解放
void DSVManager::Free(uint32_t index){
	//範囲内のインデックスのみ解放
	if (index < useIndex_){
		freeList_.push(index);
	}
}

//DSVの生成
void DSVManager::CreateDSV(ID3D12Resource* resource, uint32_t dsvIndex, DXGI_FORMAT format){
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = format;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にDSVを作る
	directXBase_->GetDevice()->CreateDepthStencilView(resource, &dsvDesc, GetCPUDescriptorHandle(dsvIndex));
}

//CPUデスクリプタハンドルの取得
D3D12_CPU_DESCRIPTOR_HANDLE DSVManager::GetCPUDescriptorHandle(uint32_t index){
	return directXBase_->GetCPUDescriptorHandle(descriptorHeap_, descriptorSize_, index);
}