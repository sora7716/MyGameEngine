#include "Context.h"
#include "Core.h"

//ゲームエンジンの核から必要な物を抽出する
void SceneContext::operator=(Core* core){
	winApi = core->GetWinApi();
	input = core->GetInput();
	textureManager = core->GetTextureManager();
	modelManager = core->GetModelManager();
	sceneManager = core->GetSceneManager();
	audioManager = core->GetAudioManager();
	imGuiManager = core->GetImGuiManager();
	tagManager = core->GetTagManager();
	lightingManager = core->GetLightingManager();
}

//ゲームエンジンの核から必要な物を抽出する
void RenderTextureContext::SetUp(Core* core){
	directXBase = core->GetDirectXBase();
	srvManager = core->GetSRVManager();
	dsvManager = core->GetDSVManager();
	rtvManager = core->GetRTVManager();
}
