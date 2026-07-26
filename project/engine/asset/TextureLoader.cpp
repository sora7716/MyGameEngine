#include "TextureLoader.h"
#include "StringUtility.h"
#include "SRVManager.h"

//エラーテクスチャを適応
void BindErrorTexture(std::string& filePath){
#ifdef _DEBUG
	filePath = "engine/resources/textures/magenta1x1.png";
#else
	filePath = "engine/resources/textures/white1x1.png";
#endif // _DEBUG
}

//テクスチャファイルの読み込み
textureLoader::LoadTextureData textureLoader::LoadTexture(SRVManager* srvManager, std::string& filePath){
	//テクスチャのファイルパスが空だった場合
	if (filePath.empty()){
		filePath = "engine/resources/textures/white1x1.png";
	}

	HRESULT hr = S_FALSE;
	DirectX::ScratchImage image{};
	std::wstring filePathW = stringUtility::ConvertString(filePath);
	DirectX::ScratchImage mipImages{};

	//読み込めるまでループ
	while (true){
		//テクスチャファイルを読み込んでプログラムを扱えるようにする
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

		if (filePathW.ends_with(L".dds")){//.ddsで終わっていたらddsとみなす。
			hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		} else{
			hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		}

		//読み込み失敗したら
		if (!SUCCEEDED(hr)){
			BindErrorTexture(filePath);
			continue;
		}

		//ミップマップの作成
		//DirectXTexでは直接的に圧縮フォーマットのMipMap生成に対応してないのでimageをそのまま使用する
		if (DirectX::IsCompressed(image.GetMetadata().format)){
			mipImages = std::move(image);
		} else{
			if (image.GetMetadata().width <= 1 && image.GetMetadata().height <= 1){
				DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, mipImages);
			} else{
				hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
			}
		}

		//読み込み失敗したら
		if (!SUCCEEDED(hr)){
			BindErrorTexture(filePath);
			continue;
		}

		//テクスチャ枚数上限チェック
		if (srvManager->TextureLimitCheck(kSRVIndexTop)){
			//上限を超えなかったら
			break;
		} else{
			assert(false);
		}
	}
	//追加したテクスチャデータの参照を取得する
	LoadTextureData result = {
		.mipImages = std::move(mipImages),
		.srvIndex = srvManager->Allocate() + kSRVIndexTop,
		.srvHandleCPU = srvManager->GetCPUDescriptorHandle(result.srvIndex),
		.srvHandleGPU = srvManager->GetGPUDescriptorHandle(result.srvIndex)
	};

	return result;
}

//テクスチャファイルのアンロード
void textureLoader::UnloadTexture(SRVManager* srvManager, std::unordered_map<std::string, TextureData>& textureDatas, const std::string& filePath){
	auto it = textureDatas.find(filePath);
	if (it != textureDatas.end()){
		srvManager->Free(it->second.srvIndex);//srvIndexの解放
		textureDatas.erase(it);//削除
	} else{
		return;//存在しない場合何もしない
	}
}
