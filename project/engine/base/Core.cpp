#include "Core.h"
#include "WinApi.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "AudioManager.h"
#include "DefaultResourceLoader.h"
#include "AbstractSceneFactory.h"
#include "TagManager.h"
#include "PipelineManager.h"
#include "LightingManager.h"
#include "RenderSystem.h"
#include "SceneFactory.h"
#include "CollisionSystem.h"

//コンストラクタ
Core::Core(){
}

//デストラクタ
Core::~Core(){
}

//初期化
void Core::Initialize(){
	//WinApi
	winApi_ = WinApi::Create(WinApi::ConstructorKey{});
	//DirectXの基盤部分
	directXBase_ = DirectXBase::Create(DirectXBase::ConstructorKey{}, winApi_.get());
	//SRVマネージャー
	srvManager_ = SRVManager::Create(SRVManager::ConstructorKey{}, directXBase_.get());
	//入力
	input_ = Input::Create(Input::ConstructorKey{}, winApi_.get());
	//テクスチャマネージャー
	textureManager_ = TextureManager::Create(TextureManager::ConstructorKey{}, directXBase_.get(), srvManager_.get());
	//モデルマネージャー
	modelManager_ = ModelManager::Create(ModelManager::ConstructorKey{}, directXBase_.get(), textureManager_.get());
	//ImGuiマネージャー
	imGuiManager_ = ImGuiManager::Create(ImGuiManager::ConstructorKey{}, winApi_.get(), directXBase_.get(), srvManager_.get());
	//パイプラインの管理
	pipelineManager_ = PipelineManager::Create(PipelineManager::ConstructorKey{}, directXBase_.get());
	pipelineManager_->CreatePSO();
	//シーンファクトリ
	sceneFactory_ = std::make_unique<SceneFactory>(AbstractSceneFactory::ConstructorKey{});
	//オーディオマネージャー
	audioManager_ = std::make_unique<AudioManager>(AudioManager::ConstructorKey{});
	//ゲームオブジェクトのリスト
	gameObjectList_ = DefaultResourceLoader::Create(DefaultResourceLoader::ConstructorKey{}, modelManager_.get(), textureManager_.get(), audioManager_.get());
	//タグの管理
	tagManager_ = TagManager::Create(TagManager::ConstructorKey{});
	//ライティングの管理
	lightingManager_ = LightingManager::Create(LightingManager::ConstructorKey{}, directXBase_.get(), srvManager_.get());
	//描画システム
	renderSystem_ = RenderSystem::Create(RenderSystem::ConstructorKey{}, directXBase_.get(), srvManager_.get(), textureManager_.get(), pipelineManager_.get(), lightingManager_.get());
	//衝突判定システム
	collisionSystem_ = std::make_unique<CollisionSystem>(CollisionSystem::ConstructorKey{});
	//シーンでの必要なものを取得
	sceneContext_ = this;

	//シーンマネージャー
	sceneManager_ = SceneManager::Create(SceneManager::ConstructorKey{}, sceneContext_, renderSystem_.get(), collisionSystem_.get());
	sceneManager_->SetSceneFactory(sceneFactory_.get());
}

//WinApiの取得
WinApi* Core::GetWinApi()const{
	return winApi_.get();
}

//DirectXの基盤部分
DirectXBase* Core::GetDirectXBase()const{
	return directXBase_.get();
}

//SRVマネージャーの取得
SRVManager* Core::GetSRVManager()const{
	return srvManager_.get();
}

//入力の取得
Input* Core::GetInput()const{
	return input_.get();
}

//テクスチャマネージャーの取得
TextureManager* Core::GetTextureManager() const{
	return textureManager_.get();
}

//モデルマネージャー
ModelManager* Core::GetModelManager() const{
	return modelManager_.get();
}

//ImGuiマネージャーの取得
ImGuiManager* Core::GetImGuiManager() const{
	return imGuiManager_.get();
}

//シーンマネージャーの取得
SceneManager* Core::GetSceneManager() const{
	return sceneManager_.get();
}

//オーディオマネージャー
AudioManager* Core::GetAudioManager() const{
	return audioManager_.get();
}

//ゲームオブジェクトのリストの取得
DefaultResourceLoader* Core::GetGameObjectList() const{
	return gameObjectList_.get();
}

//シーンファクトリの取得
AbstractSceneFactory* Core::GetSceneFactory() const{
	return sceneFactory_.get();
}

//タグの管理の取得
TagManager* Core::GetTagManager() const{
	return tagManager_.get();
}

//パイプラインの管理の取得
PipelineManager* Core::GetPipelineManager() const{
	return pipelineManager_.get();
}

//ライティングの管理の取得 
LightingManager* Core::GetLightingManager() const{
	return lightingManager_.get();
}

//描画システム
RenderSystem* Core::GetRenderSystem() const{
	return renderSystem_.get();
}

//衝突判定システムの取得
CollisionSystem* Core::GetCollisionSystem() const{
	return collisionSystem_.get();
}
