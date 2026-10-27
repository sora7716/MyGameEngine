#include "RTVManager.h"
#include "DirectXBase.h"
#include <cassert>
using namespace Microsoft::WRL;

//生成
std::unique_ptr<RTVManager> RTVManager::Create(ConstructorKey key, DirectXBase* directXBase){
	//生成
	std::unique_ptr<RTVManager>instance = std::make_unique<RTVManager>(key);
	//初期化
	instance->Initialize(directXBase);

	return instance;
}

//コンストラクタ
RTVManager::RTVManager(ConstructorKey){
}

//デストラクタ
RTVManager::~RTVManager(){
}

//初期化
void RTVManager::Initialize(DirectXBase* directXBase){
	//DirectXの基盤部分を記憶する
	directXBase_ = directXBase;
	//デスクリプタヒープの生成
	descriptorHeap_ = directXBase_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxRTVCount, false);
	//デスクリプタ1個分のサイズを取得
	descriptorSize_ = directXBase_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

}

//確保
uint32_t RTVManager::Allocate(){
	//空きリストがあるなら取得
	if (!freeList_.empty()){
		uint32_t index = freeList_.front();
		freeList_.pop();
		return index;
	}
	//上限チェック
	assert(useIndex_ < kMaxRTVCount);
	//新しい番号を割り当て
	return useIndex_++;
}

//解放
void RTVManager::Free(uint32_t index){
	//範囲内のインデックスのみ解放
	if (index < useIndex_){
		freeList_.push(index);
	}
}

//RTVの生成
void RTVManager::CreateRTV(ID3D12Resource* resource, uint32_t rtvIndex, DXGI_FORMAT format){
	//rtvDesc	
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//RTV用の設定
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//rtvHandleを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(rtvIndex);
	//レンダーターゲットビューの生成
	directXBase_->GetDevice()->CreateRenderTargetView(resource, &rtvDesc, rtvHandle);
}

//CPUデスクリプタハンドルの取得
D3D12_CPU_DESCRIPTOR_HANDLE RTVManager::GetCPUDescriptorHandle(uint32_t index){
	return directXBase_->GetCPUDescriptorHandle(descriptorHeap_, descriptorSize_, index);
}
