#pragma once
//前方宣言
class WinApi;
class Input;
class DirectXBase;
class SRVManager;
class TextureManager;
class ModelManager;
class SceneManager;
class CameraManager;
class ParticleManager;
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
	DirectXBase* directXBase;
	SRVManager* srvManager;
	TextureManager* textureManager;
	ModelManager* modelManager;
	SceneManager* sceneManager;
	CameraManager* cameraManager;
	ParticleManager* particleManager;
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
