#pragma once
//前方宣言
class WinApi;
class Input;
class TextureManager;
class ModelManager;
class SceneManager;
class AudioManager;
class ImGuiManager;
class TagManager;
class PipelineManager;
class LightingManager;
class Core;

//シーンで必要なクラス
struct SceneContext {
	WinApi* winApi;
	Input* input;
	TextureManager* textureManager;
	ModelManager* modelManager;
	SceneManager* sceneManager;
	AudioManager* audioManager;
	ImGuiManager* imGuiManager;
	TagManager* tagManager;
	PipelineManager* pipelineManager;
	LightingManager* lightingManager;

	/// <summary>
	/// ゲームエンジンの核から必要な物を抽出する
	/// </summary>
	/// <param name="core">ゲームエンジンの核</param>
	void operator=(Core* core);
};
