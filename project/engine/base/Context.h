#pragma once
//前方宣言
class Input;
class DirectXBase;
class TextureManager;
class Object3dCommon;
class Object2dCommon;
class SpriteCommon;
class ParticleCommon;
class ModelCommon;
class SceneManager;
class CameraManager;
class ParticleManager;
class AudioManager;
class ImGuiManager;
class Core;

//シーンで必要なクラス
struct SceneContext {
	Input* input;
	DirectXBase* directXBase;
	TextureManager* textureManager;
	Object3dCommon* object3dCommon;
	Object2dCommon* object2dCommon;
	SpriteCommon* spriteCommon;
	ParticleCommon* particleCommon;
	ModelCommon* modelCommon;
	SceneManager* sceneManager;
	CameraManager* cameraManager;
	ParticleManager* particleManager;
	AudioManager* audioManager;
	ImGuiManager* imguiManager;

	/// <summary>
	/// ゲームエンジンの核から必要な物を抽出する
	/// </summary>
	/// <param name="core">ゲームエンジンの核</param>
	void operator=(Core* core);
};
