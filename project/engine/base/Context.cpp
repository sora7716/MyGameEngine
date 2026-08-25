#include "Context.h"
#include "Core.h"

//ゲームエンジンの核から必要な物を抽出する
void SceneContext::operator=(Core* core){
	winApi = core->GetWinApi();
	input = core->GetInput();
	directXBase = core->GetDirectXBase();
	srvManager = core->GetSRVManager();
	textureManager = core->GetTextureManager();
	modelManager = core->GetModelManager();
	sceneManager = core->GetSceneManager();
	cameraManager = core->GetCameraManager();
	particleManager = core->GetParticleManager();
	audioManager = core->GetAudioManager();
	imGuiManager = core->GetImGuiManager();
	tagManager = core->GetTagManager();
	pipelineManager = core->GetPipelineManager();
	lightingManager = core->GetLightingManager();
}
