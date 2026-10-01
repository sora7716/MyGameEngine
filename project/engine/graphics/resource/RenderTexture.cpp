#include "RenderTexture.h"
#include "DirectXBase.h"
#include "RTVManager.h"
#include "DSVManager.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "DirectXTex/DirectXTex.h"
#include <cassert>
using namespace Microsoft::WRL;

//生成
std::unique_ptr<RenderTexture> RenderTexture::Create(const RenderTextureContext& context, uint32_t width, uint32_t height){
	std::unique_ptr<RenderTexture>instance = std::make_unique<RenderTexture>();
	instance->Initialize(context, width, height);
	return instance;
}

//コンストラクタ
RenderTexture::RenderTexture(){
}

//デストラクタ
RenderTexture::~RenderTexture(){
	//RTVの解放
	context_.rtvManager->Free(rtvIndex_);
	//DSVの解放
	context_.dsvManager->Free(dsvIndex_);
	//SRVの解放
	context_.srvManager->Free(srvIndex_ - TextureManager::kSRVIndexTop);
}

//初期化
void RenderTexture::Initialize(const RenderTextureContext& context, uint32_t width, uint32_t height){
	//レンダーテクスチャで必要なものを設定
	context_ = context;
	//横幅
	width_ = width;
	//縦幅
	height_ = height;
	//リソースの生成
	resource_ = CreateResource();

	//RTVの確保
	rtvIndex_ = context_.rtvManager->Allocate();
	//RTVの生成
	context_.rtvManager->CreateRTV(resource_.Get(), rtvIndex_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	//DSVの確保
	dsvIndex_ = context_.dsvManager->Allocate();
	//深度バッファを生成
	depthStencilResource_ = context_.directXBase->CreateDepthStencilTextureResource(width_, height_);
	//DSVの生成
	context_.dsvManager->CreateDSV(depthStencilResource_.Get(), dsvIndex_, DXGI_FORMAT_D24_UNORM_S8_UINT);

	//SRVの確保
	srvIndex_ = context_.srvManager->Allocate() + TextureManager::kSRVIndexTop;
	//メタデータ
	DirectX::TexMetadata metadata{};
	metadata.width = width_;
	metadata.height = height_;
	metadata.arraySize = 1;
	metadata.mipLevels = 1;
	metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
	//SRVの生成
	context_.srvManager->CreateSRVForTexture2D(metadata, srvIndex_, resource_.Get(), UINT(metadata.mipLevels));
}

//更新
void RenderTexture::Update(){
}

// テクスチャリソースの生成
ComPtr<ID3D12Resource> RenderTexture::CreateResource(){
	HRESULT hr = S_FALSE;
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width_;
	resourceDesc.Height = height_;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	resourceDesc.SampleDesc.Count = 1;//サンプリング。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	//2.利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProsperities{};
	heapProsperities.Type = D3D12_HEAP_TYPE_DEFAULT;//GPUに近いデフォルトを使用
	//Clear最適値を設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Color[0] = 0.1f;
	clearValue.Color[1] = 0.25f;
	clearValue.Color[2] = 0.5f;
	clearValue.Color[3] = 1.0f;
	clearValue.Format = resourceDesc.Format;
	//3.Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	hr = context_.directXBase->GetDevice()->CreateCommittedResource(
		&heapProsperities,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//初回のResourceState
		&clearValue,//Clear最適値
		IID_PPV_ARGS(&resource)//作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}