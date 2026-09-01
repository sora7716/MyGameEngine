#include "DirectXBase.h"
#include "WinApi.h"
#include "FixFPS.h"
#include "Logger.h"
#include "StringUtility.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
using namespace Microsoft::WRL;

//デストラクタ
DirectXBase::~DirectXBase() {
	//オブジェクトの開放
	CloseHandle(fenceEvent_);
}

// DirectX12の初期化
void DirectXBase::Initialize(WinApi* winApi) {
	//FPS固定初期化
	FixFPS::GetInstance()->Initialize();
	//ウィンドウズアプリケーションを受け取る
	winApi_ = winApi;
	//デバックレイヤーをオン
	DebugLayer();
	//IDXIファクトリーの生成
	dxgiFactory_ = CreateIDXGIFactory();
	//アダプターの列挙
	useAdapter_ = DecideUseAdapter();
	//デバイス生成
	device_ = CreateD3D12Device();
	//エラー時にブレークを発生させる設定
	StopExecution();
	//コマンド関連の生成
	CreateCommands();
#ifdef _DEBUG
	createSwapChainCount_ = kSwapChainCount;
#else
	createSwapChainCount_ = 1;
#endif // _DEBUG
	//スワップチェーンの生成
	for (uint32_t i = 0; i < createSwapChainCount_; i++) {
		swapChain_[i] = CreateSwapChain(WinApi::kClientWidth, WinApi::kClientHeight, kSwapChainBufferCount, i);
	}
	//深度バッファの生成
	depthStencilResource_ = CreateDepthStencilTextureResource(WinApi::kClientWidth, WinApi::kClientHeight);
	//各種デスクリプタヒープの生成
	CreateDescriptorHeap();
	//レンダーターゲットビューの初期化
	CreateRenderTargetView();
	//フェンスの初期化
	fence_ = CreateFence();
	//ビューポート矩形の初期化
	InitializeViewport();
	//シザリング矩形の初期化
	InitializeScissorRect();
	//DXCコンパイラの生成
	CreateDXCCompiler();
}

//コマンド関連の生成
void DirectXBase::CreateCommands() {
	//コマンドアローケータの生成
	commandAllocator_ = CreateCommandAllocator();
	//コマンドリストの生成
	commandList_ = CreateCommandList();
	//コマンドキューの生成
	commandQueue_ = CreateCommandQueue();
}

//各種デスクリプターヒープの生成
void DirectXBase::CreateDescriptorHeap() {
	//DescriptorSize
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);//RTV
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);//DSV
	//DescriptorHeapの作成
	//RTV
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 4, false);
	//DSV用のヒープでディスクリプタの数は1。DSVはShader内で触れるものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

//RTVの生成
void DirectXBase::CreateRenderTargetView() {
	for (uint32_t i = 0; i < createSwapChainCount_ * kSwapChainBufferCount; i++) {
		//SwapChainからResourceを引っ張ってくる
		swapChainResources_[i] = BringResourcesFromSwapChain(swapChain_[i / kSwapChainBufferCount].Get(), i%kSwapChainBufferCount);
	}

	//rtvDesc	
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//RTV用の設定
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む

	//RTVハンドルの要素数を設定
	rtvHandles_.resize(4);
	for (uint32_t i = 0; i < rtvHandles_.size(); i++) {
		//rtvHandleを取得
		rtvHandles_[i] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, i);
		//レンダーターゲットビューの生成
		device_->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc, rtvHandles_[i]);
	}

}

//ビューポート矩形の初期化
void DirectXBase::InitializeViewport() {
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = WinApi::kClientWidth;
	viewport_.Height = WinApi::kClientHeight;
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

//シザリング矩形の初期化
void DirectXBase::InitializeScissorRect() {
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApi::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApi::kClientHeight;

}

//DXCコンパイラの生成
void DirectXBase::CreateDXCCompiler() {
	HRESULT result = S_FALSE;
	//DXCユーティリティの生成
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	//DXCコンパイラの生成
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));
	//デフォルトインクルードハンドラの生成
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

// 描画開始位置
void DirectXBase::PreDraw(uint32_t swapChainIndex) {
	/*コマンドを積む*/
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_[swapChainIndex]->GetCurrentBackBufferIndex();
	backBufferIndex = backBufferIndex + kSwapChainCount * swapChainIndex;
	//今回のバリアはTransition
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	//書き込めない状態(見るだけ)
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//書き込める状態
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);
	//描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXBase::GetCPUDescriptorHandle(dsvDescriptorHeap_.Get(), descriptorSizeDSV_, 0);
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);
	//指定した色で画面をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	//ビューポート領域を設定する
	commandList_->RSSetViewports(1, &viewport_);
	//シザ－矩形の設定
	commandList_->RSSetScissorRects(1, &scissorRect_);
}

// 描画終了位置
void DirectXBase::PostDraw(uint32_t swapChainIndex) {
	HRESULT result = S_FALSE;
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_[swapChainIndex]->GetCurrentBackBufferIndex();
	backBufferIndex = backBufferIndex + kSwapChainCount * swapChainIndex;
	//画面に描く処理は全て終わり、画面に移すので、状態を遷移
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	//書き込める状態
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//書き込めない状態(見るだけ)
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
	result = commandList_->Close();
	assert(SUCCEEDED(result));

	/*コマンドをキックする*/
	//GPUにコマンドリストの実行を行わせる
	ComPtr<ID3D12CommandList> commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	//GPUとOSに画面の交換を行うように通知
	swapChain_[swapChainIndex]->Present(1, 0);
	//GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceValue_);
	if (fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
	//FPS固定
	FixFPS::GetInstance()->Update();
	//次のフレーム用のコマンドリストを準備
	//コマンドアローケータのリセット
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	//コマンドリストのリセット
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

// ディスクリプタヒープの生成
ComPtr<ID3D12DescriptorHeap> DirectXBase::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool	shaderVisible) {
	HRESULT hr = S_FALSE;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

//シェーダーのコンパイル
ComPtr<IDxcBlob> DirectXBase::CompilerShader(const std::wstring& filePath, const wchar_t* profile) {
	//1. hlslファイルを読み込む
	//これからシェーダーをコンパイルする旨をログに出す
	Logger::OutputLog(stringUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読み込む
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読み込めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知
	//2. Compilerする
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバック用の情報を埋め込む
		L"-Od",//最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler_.Get(),//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	//3. 警告・エラーが出ていないか確認する
	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Logger::OutputLog(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}
	//4. Compiler結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したらログを出す
	Logger::OutputLog(stringUtility::ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

//バッファリソースの生成
ComPtr<ID3D12Resource> DirectXBase::CreateBufferResource(size_t sizeInBytes) {
	HRESULT hr = S_FALSE;
	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;//リソースのサイズ。
	//バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

// テクスチャリソースの生成
ComPtr<ID3D12Resource> DirectXBase::CreateTextureResource(const DirectX::TexMetadata& metaDada) {
	HRESULT hr = S_FALSE;
	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metaDada.width);//Textureの幅
	resourceDesc.Height = UINT(metaDada.height);//Textureの高さ
	resourceDesc.MipLevels = UINT16(metaDada.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metaDada.arraySize);//奥行or配列Textureの配列数
	resourceDesc.Format = metaDada.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリング。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaDada.dimension);//Textureの次元数。普段使っているのは2次元
	//2.利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProsperities{};
	heapProsperities.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProsperities.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
	heapProsperities.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置
	//3.Resourceを生成する
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	hr = device_->CreateCommittedResource(
		&heapProsperities,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//初回のResourceState。Textureは基本読むだけ
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)//作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));
	return resource;
}

ComPtr<ID3D12Resource> DirectXBase::UploadTextureData(ID3D12Resource* texture, D3D12_RESOURCE_STATES& inOutState, const DirectX::ScratchImage& mipImages) {
	// 必要なら COPY_DESTへ
	if (inOutState != D3D12_RESOURCE_STATE_COPY_DEST) {
		auto toCopy = CD3DX12_RESOURCE_BARRIER::Transition(
			texture, inOutState, D3D12_RESOURCE_STATE_COPY_DEST);
		commandList_.Get()->ResourceBarrier(1, &toCopy);
		inOutState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	std::vector<D3D12_SUBRESOURCE_DATA> subResources;
	DirectX::PrepareUpload(device_.Get(), mipImages.GetImages(),
		mipImages.GetImageCount(), mipImages.GetMetadata(), subResources);

	uint64_t size = GetRequiredIntermediateSize(texture, 0, (UINT)subResources.size());
	auto intermediate = CreateBufferResource(size);

	UpdateSubresources(commandList_.Get(), texture, intermediate.Get(),
		0, 0, (UINT)subResources.size(), subResources.data());

	// GENERIC_READへ戻す
	{
		auto toRead = CD3DX12_RESOURCE_BARRIER::Transition(
			texture, inOutState, D3D12_RESOURCE_STATE_GENERIC_READ);
		commandList_.Get()->ResourceBarrier(1, &toRead);
		inOutState = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	return intermediate;
}

// RTVの指定番号のCPUデスクリプタハンドルを取得する
D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::GetRTVCPUDescriptorHandle(uint32_t index) {
	return  GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, index);
}

// RTVの指定番号のGPUデスクリプタハンドルを取得する
D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetRTVGPUDescriptorHandle(uint32_t index) {
	return  GetGPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, index);
}

// DSVの指定番号のCPUデスクリプタハンドルを取得する
D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::GetDSVCPUDescriptorHandle(uint32_t index) {
	return  GetCPUDescriptorHandle(dsvDescriptorHeap_, descriptorSizeDSV_, index);
}

// DSVの指定番号のGPUデスクリプタハンドルを取得する
D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetDSVGPUDescriptorHandle(uint32_t index) {
	return  GetGPUDescriptorHandle(dsvDescriptorHeap_, descriptorSizeDSV_, index);
}

//デバイスのゲッター
ID3D12Device* DirectXBase::GetDevice() const {
	return device_.Get();
}

//コマンドリストのゲッター
ID3D12GraphicsCommandList* DirectXBase::GetCommandList() const {
	return commandList_.Get();
}

// スワップチェーンのリソース数のゲッター
size_t DirectXBase::GetSwapChainResourceNum() const {
	return swapChainResources_.size();
}

//デプスステンシルテクスチャの取得
ID3D12Resource* DirectXBase::GetDepthStencilTexture() const {
	return depthStencilResource_.Get();
}

//コンストラクタ
DirectXBase::DirectXBase(ConstructorKey) {
}

// デスクリプターCPUハンドルのゲッター
D3D12_CPU_DESCRIPTOR_HANDLE  DirectXBase::GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

// デスクリプターGPUハンドルのゲッター
D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

//深度バッファリソースの生成
ComPtr<ID3D12Resource> DirectXBase::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	HRESULT hr = S_FALSE;

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textureの横幅
	resourceDesc.Height = height;//Textureの縦幅
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;//奥行or配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するHeap設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	hr = device_->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
		&depthClearValue,//Clear最適値
		IID_PPV_ARGS(&resource));//作成するResourceポインタのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

// IDXIファクトリーの生成
ComPtr<IDXGIFactory7> DirectXBase::CreateIDXGIFactory() {
	HRESULT result = S_FALSE;
	ComPtr<IDXGIFactory7> factory = nullptr;
	result = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	assert(SUCCEEDED(result));//生成できなかった場合止める
	return factory;
}

//使用するアダプタを決定
ComPtr<IDXGIAdapter4> DirectXBase::DecideUseAdapter() {
	HRESULT result = S_FALSE;
	ComPtr<IDXGIAdapter4> adapter = nullptr;
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; i++) {
		//アダプタ情報を受け取る
		DXGI_ADAPTER_DESC3 adapterDesc{};
		result = adapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(result));//取得できない場合止める
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力。wstringのほうなので注意
			Logger::OutputLog(stringUtility::ConvertString(std::format(L"Use Adapter : {}\n", adapterDesc.Description)));
			break;
		}
		adapter = nullptr;//ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタが見つからなかった場合止める
	assert(adapter != nullptr);
	return adapter;
}

// D3D12デバイスの生成
ComPtr<ID3D12Device> DirectXBase::CreateD3D12Device() {
	HRESULT result = S_FALSE;
	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(result)) {
			//生成できたのでログを出力を行ってループを抜ける
			Logger::OutputLog(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスがうまく生成できなかった場合止める
	assert(device != nullptr);
	Logger::OutputLog("Complete create D3D12Device!!!\n");//初期化完了のログをだす
	return device;
}

//コマンドキューの生成
ComPtr<ID3D12CommandQueue> DirectXBase::CreateCommandQueue() {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));
	return commandQueue;
}

// コマンドアローケータの生成
ComPtr<ID3D12CommandAllocator> DirectXBase::CreateCommandAllocator() {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアローケータがうまく生成できなかった場合止める
	assert(SUCCEEDED(result));
	return commandAllocator;
}

// コマンドリストの生成
ComPtr<ID3D12GraphicsCommandList> DirectXBase::CreateCommandList() {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList));
	//コマンドリストがうまく生成できなかった場合止める
	assert(SUCCEEDED(result));
	return commandList;
}

//スワップチェーンの生成
ComPtr<IDXGISwapChain4> DirectXBase::CreateSwapChain(int32_t windowWidth, int32_t windowHeight, uint32_t bufferSize, uint32_t hwndIndex) {
	HRESULT result = S_FALSE;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = windowWidth;//画面の横幅
	swapChainDesc.Height = windowHeight;//画面の縦幅
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用
	swapChainDesc.BufferCount = bufferSize;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら、中身を破棄
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	result = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApi_->GetHwnd(hwndIndex), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(result));
	return swapChain;
}

// SwapChainからResourceを引っ張ってくる
ComPtr<ID3D12Resource> DirectXBase::BringResourcesFromSwapChain(IDXGISwapChain4* swapChain, UINT num) {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12Resource> swapChainResource = nullptr;
	result = swapChain->GetBuffer(num, IID_PPV_ARGS(&swapChainResource));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(result));
	return swapChainResource;
}

//Fenceを作成する
ComPtr<ID3D12Fence> DirectXBase::CreateFence() {
	HRESULT result = S_FALSE;
	ComPtr<ID3D12Fence> fence = nullptr;
	fenceValue_ = 0;
	result = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(result));

	//FenceのSignalを待つためのイベントを作成
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
	return fence;
}

//デバックレイヤー
void DirectXBase::DebugLayer() {
#ifdef _DEBUG
	debugController_ = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		//デバックレイヤーを有効化する
		debugController_->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif // _DEBUG
}

// 実行を停止する(エラー・警告の場合)
void DirectXBase::StopExecution() {
#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		//解放
		infoQueue->Release();
	}
#endif // _DEBUG
}
