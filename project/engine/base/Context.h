#pragma once
//前方宣言
class WinApi;
class Input;
class TextureManager;
class ModelManager;
class SceneManager;
class AudioManager;
class ImGuiManager;
class Core;
class DirectXBase;
class SRVManager;
class DSVManager;
class RTVManager;

//シーンで必要なクラス
struct SceneContext{
	Input* input;
	TextureManager* textureManager;
	ModelManager* modelManager;
	SceneManager* sceneManager;
	AudioManager* audioManager;
	ImGuiManager* imGuiManager;

	/// <summary>
	/// ゲームエンジンの核から必要な物を抽出する
	/// </summary>
	/// <param name="core">ゲームエンジンの核</param>
	void operator=(Core* core);
};

/// <summary>
/// レンダーテクスチャで必要なもの
/// </summary>
struct RenderTextureContext{
	DirectXBase* directXBase = nullptr;
	SRVManager* srvManager = nullptr;
	DSVManager* dsvManager = nullptr;
	RTVManager* rtvManager = nullptr;

	/// <summary>
	/// ゲームエンジンの核から必要な物を抽出する
	/// </summary>
	/// <param name="core">ゲームエンジンの核</param>
	void SetUp(Core* core);
};
