#include "Context.h"
#include "engine/base/Core.h"

//ゲームエンジンの核から必要な物を抽出する
void SceneContext::operator=(Core* core){
	winApi = core->GetWinApi();
	input = core->GetInput();
	directXBase = core->GetDirectXBase();
	srvManager = core->GetSRVManager();
	textureManager = core->GetTextureManager();
	modelManager = core->GetModelManager();
	spriteCommon = core->GetSpriteCommon();
	particleCommon = core->GetParticleCommon();
	modelCommon = core->GetModelCommon();
	sceneManager = core->GetSceneManager();
	cameraManager = core->GetCameraManager();
	particleManager = core->GetParticleManager();
	audioManager = core->GetAudioManager();
	imGuiManager = core->GetImGuiManager();
	tagManager = core->GetTagManager();
	pipelineManager = core->GetPipelineManager();
	lightingManager = core->GetLightingManager();
}
