#include "TextureManager.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "StringUtility.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
using namespace Microsoft::WRL;

//ImGuiで0番目を使用するため、1番目から使用
uint32_t TextureManager::kSRVIndexTop = 1;

//初期化
void TextureManager::Initialize(DirectXBase* directXBase, SRVManager* srvManager) {
	//DirectXの基盤部分を記録する
	directXBase_ = directXBase;
	//SRVの管理を記録する
	srvManager_ = srvManager;
	//SRVの数と同数
	textureDatas_.reserve(SRVManager::kMaxSRVCount);
}

//テクスチャファイルの読み込み
void TextureManager::LoadTexture(std::string& filePath) {
	//テクスチャのファイルパスが空だった場合
	if (filePath.empty()) {
		filePath = "engine/resources/textures/white1x1.png";
	}

	HRESULT hr = S_FALSE;
	DirectX::ScratchImage image{};
	std::wstring filePathW = stringUtility::ConvertString(filePath);
	DirectX::ScratchImage mipImages{};

	//読み込めるまでループ
	while (true) {

		//読み込み済みテクスチャを検索
		if (textureDatas_.contains(filePath)) {
			//読み込み済みなら早期リターン
			return;
		}
		//テクスチャファイルを読み込んでプログラムを扱えるようにする
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

		if (filePathW.ends_with(L".dds")) {//.ddsで終わっていたらddsとみなす。
			hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		} else {
			hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		}

		//読み込み失敗したら
		if (!SUCCEEDED(hr)) {
			BindErrorTexture(filePath);
			continue;
		}

		//ミップマップの作成
		//DirectXTexでは直接的に圧縮フォーマットのMipMap生成に対応してないのでimageをそのまま使用する
		if (DirectX::IsCompressed(image.GetMetadata().format)) {
			mipImages = std::move(image);
		} else {
			if (image.GetMetadata().width <= 1 && image.GetMetadata().height <= 1) {
				DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, mipImages);
			} else {
				hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
			}
		}

		//読み込み失敗したら
		if (!SUCCEEDED(hr)) {
			BindErrorTexture(filePath);
			continue;
		}

		//テクスチャ枚数上限チェック
		if (srvManager_->TextureLimitCheck(kSRVIndexTop)) {
			//上限を超えなかったら
			break;
		} else {
			assert(false);
		}
	}
	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas_[filePath];
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = directXBase_->CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = directXBase_->UploadTextureData(textureData.resource.Get(), textureData.state, mipImages);
	//SRVの確保
	textureData.srvIndex = srvManager_->Allocate() + kSRVIndexTop;
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);
	//SRVの設定
	srvManager_->CreateSRVForTexture2D(textureData.metadata, textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));
}

//テクスチャファイルのアンロード
void TextureManager::UnloadTexture(const std::string& filePath) {
	auto it = textureDatas_.find(filePath);
	if (it != textureDatas_.end()) {
		srvManager_->Free(it->second.srvIndex);//srvIndexの解放
		textureDatas_.erase(it);//削除
	} else {
		return;//存在しない場合何もしない
	}
}

//文字テクスチャなどをCPUメモリから作成
void TextureManager::CreateTextureFromMemoryBGRA(const std::string& key, const void* pixelsBGRA, uint32_t width, uint32_t height, uint32_t strideBytes) {
	if (textureDatas_.contains(key)) {
		return;//テクスチャデータに存在したら早期リターン
	}

	//SRV上限チェック
	assert(srvManager_->TextureLimitCheck(kSRVIndexTop));

	//ScratchImageを作ってピクセルを詰める
	DirectX::ScratchImage img{};
	HRESULT hr = img.Initialize2D(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1);
	assert(SUCCEEDED(hr));

	const DirectX::Image* im = img.GetImage(0, 0, 0);
	uint8_t* dst = img.GetPixels();

	//row by row copy (stride対応)
	for (uint32_t y = 0; y < height; y++) {
		memcpy(dst + size_t(im->rowPitch) * y,
			(const uint8_t*)pixelsBGRA + size_t(strideBytes) * y,
			size_t(width) * 4);
	}

	TextureData& textureData = textureDatas_[key];
	textureData.metadata = img.GetMetadata();
	textureData.resource = directXBase_->CreateTextureResource(textureData.metadata);
	textureData.intermediateResource = directXBase_->UploadTextureData(textureData.resource.Get(), textureData.state, img);

	textureData.srvIndex = srvManager_->Allocate() + kSRVIndexTop;
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	srvManager_->CreateSRVForTexture2D(textureData.metadata, textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));
}

//文字テクスチャなどをCPUメモリからの更新
void TextureManager::UpdateTextureFromMemoryBGRA(const std::string& key, const void* pixelsBGRA, uint32_t width, uint32_t height, uint32_t strideBytes) {

	//まだ無ければ新規作成
	if (!textureDatas_.contains(key)) {
		CreateTextureFromMemoryBGRA(key, pixelsBGRA, width, height, strideBytes);
		return;
	}

	TextureData& textureData = textureDatas_[key];

	//サイズが違うなら作り直し
	if (textureData.metadata.width != width || textureData.metadata.height != height) {
		//一旦消して作り直す
		UnloadTexture(key);
		CreateTextureFromMemoryBGRA(key, pixelsBGRA, width, height, strideBytes);
		return;
	}

	//同じサイズ(resourceは再利用してuploadだけやり直す)
	DirectX::ScratchImage img{};
	HRESULT hr = img.Initialize2D(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1);
	assert(SUCCEEDED(hr));

	const DirectX::Image* im = img.GetImage(0, 0, 0);
	uint8_t* dst = img.GetPixels();
	for (uint32_t y = 0; y < height; y++) {
		memcpy(dst + size_t(im->rowPitch) * y,
			(const uint8_t*)pixelsBGRA + size_t(strideBytes) * y,
			size_t(width) * 4);
	}


	//ここでupload resourceを作り出す
	textureData.intermediateResource = directXBase_->UploadTextureData(textureData.resource.Get(), textureData.state, img);
}

// メタデータの取得
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {
	// TODO: return ステートメントをここに挿入します
	return textureDatas_[filePath].metadata;
}

// SRVインデックスの取得
uint32_t TextureManager::GetSRVIndex(const std::string& filePath) {
	return textureDatas_[filePath].srvIndex;
}

// GPUハンドルの取得
D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRVHandleGPU(const std::string& filePath) {
	return textureDatas_[filePath].srvHandleGPU;
}

//エラーテクスチャを適応
void TextureManager::BindErrorTexture(std::string& filePath) {
#ifdef _DEBUG
	filePath = "engine/resources/textures/magenta1x1.png";
#else
	filePath = "engine/resources/textures/white1x1.png";
#endif // _DEBUG
}

//コンストラクタ
TextureManager::TextureManager(ConstructorKey) {}
