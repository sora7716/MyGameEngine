#include "Core.h"
#include "engine/scene/SceneFactory.h"

//コンストラクタ
Core::Core(){
}

//デストラクタ
Core::~Core(){
}

//初期化
void Core::Initialize(){
	//WinApi
	winApi_ = std::make_unique<WinApi>(WinApi::ConstructorKey{});
	winApi_->Initialize();
	//DirectXの基盤部分
	directXBase_ = std::make_unique<DirectXBase>(DirectXBase::ConstructorKey{});
	directXBase_->Initialize(winApi_.get());
	//SRVマネージャー
	srvManager_ = std::make_unique<SRVManager>(SRVManager::ConstructorKey{});
	srvManager_->Initialize(directXBase_.get());
	//入力
	input_ = std::make_unique<Input>(Input::ConstructorKey{});
	input_->Initialize(winApi_.get());
	//テクスチャマネージャー
	textureManager_ = std::make_unique<TextureManager>(TextureManager::ConstructorKey{});
	textureManager_->Initialize(directXBase_.get(), srvManager_.get());
	//モデルの共通部分
	modelCommon_ = std::make_unique<ModelCommon>(ModelCommon::ConstructorKey{});
	modelCommon_->Initialize(directXBase_.get(), textureManager_.get());
	//モデルマネージャー
	modelManager_ = std::make_unique<ModelManager>(ModelManager::ConstructorKey{});
	modelManager_->Initialize(modelCommon_.get());
	//ImGuiマネージャー
	imguiManager_ = std::make_unique<ImGuiManager>(ImGuiManager::ConstructorKey{});
	imguiManager_->Initialize(winApi_.get(), directXBase_.get(), srvManager_.get());
	//カメラマナージャー
	cameraManager_ = std::make_unique<CameraManager>(CameraManager::ConstructorKey{});
	cameraManager_->Initialize(directXBase_.get());
	//パイプラインの管理
	pipelineManager_ = std::make_unique<PipelineManager>(PipelineManager::ConstructorKey{});
	pipelineManager_->Initialize(directXBase_.get());
	pipelineManager_->CreatePSO();
	//スプライトの共通部分
	spriteCommon_ = std::make_unique<SpriteCommon>(SpriteCommon::ConstructorKey{});
	spriteCommon_->Initialize(directXBase_.get(), textureManager_.get());
	//3Dオブジェクトの共通部分
	object3dCommon_ = std::make_unique<Object3dCommon>(Object3dCommon::ConstructorKey{});
	object3dCommon_->Initialize(directXBase_.get(), srvManager_.get(), textureManager_.get(), modelManager_.get());
	//パーティクルの共通部分
	particleCommon_ = std::make_unique<ParticleCommon>(ParticleCommon::ConstructorKey{});
	particleCommon_->Initialize(directXBase_.get(), srvManager_.get(), textureManager_.get());
	//シーンファクトリ
	sceneFactory_ = std::make_unique<SceneFactory>(AbstractSceneFactory::ConstructorKey{});
	//オーディオマネージャー
	audioManager_ = std::make_unique<AudioManager>(AudioManager::ConstructorKey{});
	//パーティクルマネージャー
	particleManager_ = std::make_unique<ParticleManager>(ParticleManager::ConstructorKey{});
	//ゲームオブジェクトのリスト
	gameObjectList_ = std::make_unique<GameObjectList>(GameObjectList::ConstructorKey{});
	gameObjectList_->Initialize(this);
	//タグの管理
	tagManager_ = std::make_unique<TagManager>(TagManager::ConstructorKey{});
	tagManager_->Initialize();
	//ライティングの管理
	lightingManager_ = std::make_unique<LightingManager>(LightingManager::ConstructorKey{});
	lightingManager_->Initialize(directXBase_.get(), srvManager_.get());
	//シーンでの必要なものを取得
	sceneContext_ = this;
	//シーンマネージャー
	sceneManager_ = std::make_unique<SceneManager>(SceneManager::ConstructorKey{});
	sceneManager_->Initialize(sceneContext_);
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
	return imguiManager_.get();
}

//カメラマネージャーの取得
CameraManager* Core::GetCameraManager()const{
	return cameraManager_.get();
}

//スプライトの共通部分の取得
SpriteCommon* Core::GetSpriteCommon() const{
	return spriteCommon_.get();
}

//3Dオブジェクトの共通部分の取得
Object3dCommon* Core::GetObject3dCommon() const{
	return object3dCommon_.get();
}

//パーティクルの共通部分の取得
ParticleCommon* Core::GetParticleCommon() const{
	return particleCommon_.get();
}

//モデルの共通部分
ModelCommon* Core::GetModelCommon() const{
	return modelCommon_.get();
}

//シーンマネージャーの取得
SceneManager* Core::GetSceneManager() const{
	return sceneManager_.get();
}

//オーディオマネージャー
AudioManager* Core::GetAudioManager() const{
	return audioManager_.get();
}

//パーティクルマネージャーの取得
ParticleManager* Core::GetParticleManager() const{
	return particleManager_.get();
}

//ゲームオブジェクトのリストの取得
GameObjectList* Core::GetGameObjectList() const{
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
