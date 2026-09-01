#pragma once
#include "RenderData.h"
#include <unordered_map>

//前方宣言
class SRVManager;

/// <summary>
/// テクスチャの読み込み
/// </summary>
namespace textureLoader{
	//ImGuiで0番目を使用するため、1番目から使用
	const uint32_t kSRVIndexTop = 1;

	//読み込む際に取得するデータ
	struct LoadTextureData{
		DirectX::ScratchImage mipImages;
		uint32_t srvIndex;//SRVインデックス
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = {};//SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = {};//描画コマンドに必要なGPUハンドル
	};

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャのファイルパス</param>
	LoadTextureData LoadTexture(SRVManager* srvManager, std::string& filePath);

	/// <summary>
	/// テクスチャファイルのアンロード
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void UnloadTexture(SRVManager* srvManager, std::unordered_map<std::string, TextureData>& textureDatas, const std::string& filePath);
}

