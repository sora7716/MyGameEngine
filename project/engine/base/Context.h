#pragma once
//前方宣言
class WinApi;
class Input;
class DirectXBase;
class SRVManager;
class TextureManager;
class ModelManager;
class Object2dCommon;
class SpriteCommon;
class ParticleCommon;
class ModelCommon;
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
	Object2dCommon* object2dCommon;
	SpriteCommon* spriteCommon;
	ParticleCommon* particleCommon;
	ModelCommon* modelCommon;
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
