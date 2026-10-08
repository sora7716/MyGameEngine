#include "SceneManager.h"
#include "ImGuiManager.h"
#include "DebugEditor.h"
#include "TagManager.h"
#include "RenderSystem.h"
#include "CollisionSystem.h"
#include "Object3d.h"
#include "Logger.h"
#include <cassert>

//生成
std::unique_ptr<SceneManager> SceneManager::Create(ConstructorKey key, const SceneContext& sceneContext, RenderSystem* renderSystem, CollisionSystem* collisionSystem){
	//生成
	std::unique_ptr<SceneManager>instance = std::make_unique<SceneManager>(key);
	//初期化
	instance->Initialize(sceneContext, renderSystem, collisionSystem);

	return instance;
}

//コンストラクタ
SceneManager::SceneManager(ConstructorKey){}

//デストラクタ
SceneManager::~SceneManager(){
	scene_->Finalize();
	delete scene_;
}

//初期化
void SceneManager::Initialize(const SceneContext& sceneContext, RenderSystem* renderSystem, CollisionSystem* collisionSystem){
	//必要な物を記録
	sceneContext_ = sceneContext;
	//描画システムを記録
	assert(renderSystem);
	renderSystem_ = renderSystem;
	//衝突判定システムを記録
	assert(collisionSystem);
	collisionSystem_ = collisionSystem;
	//シーンマネージャだけ自分から渡す
	sceneContext_.sceneManager = this;
	//デバッグエディターの生成と初期化
	debugEditor_ = std::make_unique<DebugEditor>();
	debugEditor_->Initialize(sceneContext_.textureManager, sceneContext_.tagManager);
}

//更新
void SceneManager::Update(){
	//次のシーンの予約があるなら
	if (nextScene_){
		//旧シーンの終了
		if (scene_){
			//デバッグエディタの初期化
			debugEditor_->Initialize(sceneContext_.textureManager, sceneContext_.tagManager);
			//旧シーンの解放
			scene_->Finalize();
			delete scene_;
		}
		//シーンの切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		//シーンに必要な情報の設定
		scene_->SetUp(sceneContext_);
		//次のシーン
		scene_->Initialize();
		//ゲームオブジェクト一覧をDebugEditorに設定
		debugEditor_->SetGameObjects(scene_->GetGameObjects());
		//デバッグカメラをDebugEditorに設定
		debugEditor_->SetDebugCamera(scene_->GetDebugCamera());
	}
	//衝突判定システムの更新
	collisionSystem_->Update(scene_->GetGameObjects());
	//デバッグエディタの更新
	debugEditor_->Update();
	//更新
	scene_->Update();
	//デバッグ操作が有効かを設定
	scene_->SetIsDebugControlEnabled(debugEditor_->IsSceneViewHovered());
	//アプリケーションが有効かを設定
	scene_->SetIsAppInputEnabled(debugEditor_->IsPreviewHovered());
}

//デバッグ
void SceneManager::Debug(D3D12_GPU_DESCRIPTOR_HANDLE sceneHandle, D3D12_GPU_DESCRIPTOR_HANDLE previewHandle){
	(void)sceneHandle;
	(void)previewHandle;
#ifdef USE_IMGUI
	sceneContext_.imGuiManager->Begin();
	//デバッグエディタの描画
	debugEditor_->Draw(sceneHandle, previewHandle);

	//親子付けを要求
	ParentRequest parentRequest = {};
	if (debugEditor_->ConsumeParentRequest(parentRequest)){
		GameObject* parent = parentRequest.parent;
		GameObject* child = parentRequest.child;

		//親と子が存在するか
		if (parent && child){
			Object3d* parentObject3d = parentRequest.parent->GetComponent<Object3d>();
			Object3d* childObject3d = parentRequest.child->GetComponent<Object3d>();

			//親と子でObject3dを取得できた場合
			if (parentObject3d && childObject3d){
				//親子付けを行う
				bool isAttached = childObject3d->AttachTo(parentObject3d, parentRequest.nodePath);
				if (!isAttached){
					Logger::OutputLog("親子付けが失敗しました");
				}
			} else{
				Logger::OutputLog("親または子でObject3d取得に失敗しました");
			}
		} else{
			Logger::OutputLog("親または子でGameObjectがNullでした");
		}
	}

	//親子付けを解除
	GameObject* detachObject = nullptr;
	if (debugEditor_->ConsumeDetachRequest(detachObject)){
		//ターゲットがなければ
		if (!detachObject){
			return;
		}

		//Object3dが取得できたか
		Object3d* object3d = detachObject->GetComponent<Object3d>();
		if (object3d){
			//親子付けを解除
			object3d->DetachParent();
		} else{
			Logger::OutputLog("親子付け解除する対象のObject3d取得に失敗しました");
		}
	}

	//生成要求
	if (debugEditor_->ConsumeCreateRequest()){
		GameObject* newGameObject = scene_->CreateGameObject();
		//生成したGameObjectを選択
		debugEditor_->SelectGameObject(newGameObject);
	}

	//複製要求
	GameObject* duplicateObject = nullptr;
	if (debugEditor_->ConsumeDuplicateRequest(duplicateObject)){
		scene_->DuplicateGameObject(duplicateObject);
	}

	//削除要求
	GameObject* deleteObject = nullptr;
	if (debugEditor_->ConsumeDeleteRequest(deleteObject)){
		scene_->DeleteGameObject(deleteObject, collisionSystem_);
	}

	//ゲームオブジェクトの移動要求
	uint32_t fromIndex = 0;
	uint32_t toIndex = 0;
	if (debugEditor_->ConsumeMoveGameObjectRequest(fromIndex, toIndex)){
		scene_->MoveGameObject(fromIndex, toIndex);
	}

	//タグの名前変更を要求
	std::string oldTag = "\0";
	std::string newTag = "\0";
	if (debugEditor_->ConsumeRenameTagRequest(oldTag, newTag)){
		if (sceneContext_.tagManager->RenameTag(oldTag, newTag)){
			scene_->ReplaceGameObjectTag(oldTag, newTag);
		}
	}

	//タグの削除を要求
	std::string deleteTag = "\0";
	if (debugEditor_->ConsumeDeleteTagRequest(deleteTag)){
		if (sceneContext_.tagManager->RemoveTag(deleteTag)){
			scene_->ReplaceGameObjectTag(deleteTag, TagManager::kDefaultTagName);
		}
	}

	//シーンのデバッグ
	scene_->Debug();
	sceneContext_.imGuiManager->End();
#endif // USE_IMGUI
}

//描画
void SceneManager::Draw(CameraMode cameraMode){
	const std::vector<std::unique_ptr<GameObject>>& gameObjects = scene_->GetGameObjects();
	//カメラの追加
	renderSystem_->CollectActiveCameras(gameObjects);
	//カメラのモードを設定
	renderSystem_->SetCameraMode(cameraMode);

	//Object3dの追加
	renderSystem_->CollectActiveObject3ds(gameObjects);
	//SkyBoxの追加
	renderSystem_->CollectActiveSkyBox(gameObjects);
	//Spriteの描画
	renderSystem_->CollectActiveSprites(gameObjects);
	//DebugDrawの描画
	renderSystem_->CollectActiveDebugDraw(gameObjects);
	//ParticleSystemの描画
	renderSystem_->CollectActiveParticleSystems(gameObjects);
}

//ゲーム画面の描画
void SceneManager::PreviewDraw(){
	//描画
	Draw(CameraMode::kMain);
}

//デバッグ画面の描画
void SceneManager::SceneDraw(){
	//描画
	Draw(CameraMode::kDebug);
}

//シーンファクトリーのセッター
void SceneManager::SetSceneFactory(AbstractSceneFactory* sceneFactory){
	sceneFactory_ = sceneFactory;
}

//シーンの切り替え
void SceneManager::ChangeScene(const std::string& sceneName){
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}