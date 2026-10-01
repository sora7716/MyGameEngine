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
	colorResource_ = CreateResource();

	//RTVの確保
	rtvIndex_ = context_.rtvManager->Allocate();
	//RTVの生成
	context_.rtvManager->CreateRTV(colorResource_.Get(), rtvIndex_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

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
	context_.srvManager->CreateSRVForTexture2D(metadata, srvIndex_, colorResource_.Get(), UINT(metadata.mipLevels));

	//ビューポートの初期化
	InitializeViewport();

	//シザー矩形の初期化
	InitializeScissorRect();
}

//更新
void RenderTexture::Update(){
}

//描画開始
void RenderTexture::PreDraw(){
	//DirectXの基盤部分を取得
	DirectXBase* directXBase = context_.directXBase;

	//RTVの管理を取得
	RTVManager* rtvManager = context_.rtvManager;
	//RTVのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager->GetCPUDescriptorHandle(rtvIndex_);

	//DSVの管理を取得
	DSVManager* dsvManager = context_.dsvManager;
	//DSVのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvManager->GetCPUDescriptorHandle(dsvIndex_);

	//今回のバリアはTransition
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。
	barrier.Transition.pResource = colorResource_.Get();
	//書き込めない状態(見るだけ)
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//書き込める状態
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	directXBase->GetCommandList()->ResourceBarrier(1, &barrier);

	//描画先のRTVを設定する
	directXBase->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	//指定した色で画面をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
	directXBase->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	directXBase->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//ビューポート領域を設定する
	directXBase->GetCommandList()->RSSetViewports(1, &viewport_);

	//シザ－矩形の設定
	directXBase->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

//描画終了
void RenderTexture::PostDraw(){
	//DirectXの基盤部分を取得
	DirectXBase* directXBase = context_.directXBase;
	//画面に描く処理は全て終わり、画面に移すので、状態を遷移
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = colorResource_.Get();
	//書き込める状態
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//書き込めない状態(見るだけ)
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	directXBase->GetCommandList()->ResourceBarrier(1, &barrier);
}

//GPUデスクリプタハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetGPUDescriptorHandle() const{
	return context_.srvManager->GetGPUDescriptorHandle(srvIndex_);
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

//ビューポート矩形の初期化
void RenderTexture::InitializeViewport(){
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(width_);
	viewport_.Height = static_cast<float>(height_);
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

//シザリング矩形の初期化
void RenderTexture::InitializeScissorRect(){
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = static_cast<LONG>(viewport_.TopLeftX);
	scissorRect_.right = static_cast<LONG>(viewport_.TopLeftX + viewport_.Width);
	scissorRect_.top = static_cast<LONG>(viewport_.TopLeftY);
	scissorRect_.bottom = static_cast<LONG>(viewport_.TopLeftY + viewport_.Height);
}