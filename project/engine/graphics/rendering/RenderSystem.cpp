#include "RenderSystem.h"
#include "DirectXBase.h"
#include "PipelineManager.h"
#include "LightingManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "Object3d.h"
#include "Object3dRenderer.h"
#include "Culling.h"
#include "Model.h"
#include "Mesh.h"
#include "SkyBox.h"
#include "SkyBoxRenderer.h"
#include "Sprite.h"
#include "SpriteRenderer.h"
#include "BaseShape.h"
#include "DebugDrawRenderer.h"
#include "ParticleSystem.h"
#include "ParticleRenderer.h"
#include "DebugCameraController.h"
#include "CameraRenderer.h"
#include <algorithm>

//生成
std::unique_ptr<RenderSystem> RenderSystem::Create(ConstructorKey key, DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, PipelineManager* pipelineManager, LightingManager* lightingManager){
	//生成
	std::unique_ptr<RenderSystem>instance = std::make_unique<RenderSystem>(key);
	//初期化
	instance->Initialize(directXBase, srvManager, textureManager, pipelineManager, lightingManager);

	return instance;
}

//コンストラクタ
RenderSystem::RenderSystem(ConstructorKey){
}

//デストラクタ
RenderSystem::~RenderSystem(){
}

//初期化
void RenderSystem::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, PipelineManager* pipelineManager, LightingManager* lightingManager){
	//DirectXの基盤部分の記録
	assert(directXBase);
	directXBase_ = directXBase;
	//パイプラインの管理の記録
	assert(pipelineManager);
	pipelineManager_ = pipelineManager;
	//ライティングの管理の記録
	assert(lightingManager);
	lightingManager_ = lightingManager;
	//スプライトのレンダラー
	spriteRenderer_ = SpriteRenderer::Create(SpriteRenderer::ConstructorKey{}, directXBase, textureManager);
	//Object3dのレンダラー
	object3dRenderer_ = Object3dRenderer::Create(Object3dRenderer::ConstructorKey{}, directXBase, srvManager, textureManager);
	//スカイボックスのレンダラー
	skyBoxRenderer_ = SkyBoxRenderer::Create(SkyBoxRenderer::ConstructorKey{}, directXBase, textureManager);
	//デバッグ描画のレンダラー
	debugDrawRenderer_ = DebugDrawRenderer::Create(DebugDrawRenderer::ConstructorKey{}, directXBase);
	//パーティクルのレンダラー
	particleRenderer_ = ParticleRenderer::Create(ParticleRenderer::ConstructorKey{}, directXBase, srvManager, textureManager);
	//カメラのレンダラー
	cameraRenderer_ = CameraRenderer::Create(CameraRenderer::ConstructorKey{}, directXBase);
}

//描画
void RenderSystem::Draw(){
	//Object3d
	for (uint32_t i = 0; i < object3dRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(object3dRenderer_->GetBlendMode(i), PipelineType::kObject3d);
		//ライティングの設定
		lightingManager_->Bind();
		//カメラの設定
		cameraRenderer_->Bind(selectCamera_.index, 4);
		//Object3dの描画
		object3dRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	object3dRenderer_->Reset();

	//Particle
	for (uint32_t i = 0; i < particleRenderer_->GetRenderDataSize(); i++){
		//描画の開始
		PreDraw(particleRenderer_->GetBlendMode(i), PipelineType::kParticle);
		//カメラの設定
		cameraRenderer_->Bind(selectCamera_.index, 3);
		//Particleの描画
		particleRenderer_->Draw(i, selectCamera_.camera);
	}
	//描画オブジェクトのリセット
	particleRenderer_->Reset();

	//DebugDraw
	for (uint32_t i = 0; i < debugDrawRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(debugDrawRenderer_->GetBlendMode(i));
		//カメラの設定
		cameraRenderer_->Bind(selectCamera_.index, 2);
		//DebugDrawの描画
		debugDrawRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	debugDrawRenderer_->Reset();

	//スカイボックス
	for (uint32_t i = 0; i < skyBoxRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(skyBoxRenderer_->GetBlendMode(i), PipelineType::kSkyBox);
		//カメラの設定
		cameraRenderer_->Bind(selectCamera_.index, 3);
		//スカイボックスの描画
		skyBoxRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	skyBoxRenderer_->Reset();

	//スプライト
	for (uint32_t i = 0; i < spriteRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(spriteRenderer_->GetBlendMode(i), PipelineType::kSprite);
		//Spriteの描画
		spriteRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	spriteRenderer_->Reset();

	//カメラのリセット
	cameraRenderer_->Reset();

}

//描画に有効なObject3dを集める
void RenderSystem::CollectActiveObject3ds(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//今回描画で使用するカメラ
	Camera* renderCamera = selectCamera_.camera;

	//object3dsをクリア
	activeObject3ds_.clear();

	//描画で使用するカメラがなければ
	if (!renderCamera){
		object3dBatches_.clear();
		return;
	}
	//カメラのワールド座標を取得
	Vector3 cameraWorldPos = renderCamera->GetWorldPos();

	//カリングの生成
	std::unique_ptr<Culling> culling = Culling::Create(renderCamera);

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//object3dを取得
		Object3d* object3d = gameObject->GetComponent<Object3d>();

		//object3dがNullか
		if (!object3d){
			continue;
		}

		//object3dが有効状態か
		if (!object3d->IsEnabled()){
			continue;
		}

		//モデルが設定されているか
		if (!object3d->HasModel()){
			continue;
		}


		//LODを更新
		//object3dのワールド座標を取得
		Vector3 object3dWorldPos = object3d->GetWorldPos();
		object3d->UpdateLOD((object3dWorldPos - cameraWorldPos).Length());

		//モデルを取得
		Model* model = object3d->GetRenderModel();

		//モデルがなければ
		if (!model){
			continue;
		}

		//視錐台カリング
		bool isVisible = false;
		const Matrix4x4 renderWorld = object3d->MakeRenderWorldMatrix(renderCamera->GetWorldMatrix());
		//メッシュごと
		for (const std::unique_ptr<Mesh>& mesh : model->GetMeshes()){
			//メッシュがなければ
			if (!mesh){
				continue;
			}

			//カリングを行う
			if (culling->IsVisibleInFrustum(mesh->GetAABB(), renderWorld)){
				isVisible = true;
				break;
			}
		}

		//isVisibleがfalseなら
		if (!isVisible){
			continue;
		}

		//Object3dsに追加
		activeObject3ds_.push_back(object3d);
	}

	//収集したObject3dをグループ分け
	BuildObject3dBatches();
	//トランスフォーメーションデータの構築
	BuildTransformationData();
	//作成したバッチをレンダラーに送信
	SubmitObject3dBatches();
}

//描画に有効なSkyBoxを集める
void RenderSystem::CollectActiveSkyBox(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//今回描画で使用するカメラ
	Camera* renderCamera = selectCamera_.camera;
	//描画に有効なSkyBoxをリセット
	activeSkyBox_ = nullptr;

	//描画カメラがNullだったら
	if (!renderCamera){
		return;
	}

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//SkyBoxを取得
		SkyBox* skyBox = gameObject->GetComponent<SkyBox>();

		//SkyBoxがNullか
		if (!skyBox){
			continue;
		}

		//SkyBoxが有効状態か
		if (!skyBox->IsEnabled()){
			continue;
		}

		//activeSkyBoxを設定
		activeSkyBox_ = skyBox;
		break;
	}

	if (activeSkyBox_){
		skyBoxRenderer_->AddRenderData(activeSkyBox_->GetRenderData());
	}
}

//描画に有効なSpriteを集める
void RenderSystem::CollectActiveSprites(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//描画に有効なSpriteをリセット
	activeSprites_.clear();

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//Spriteを取得
		Sprite* sprite = gameObject->GetComponent<Sprite>();

		//オブジェクト3dがNullか
		if (!sprite){
			continue;
		}

		//オブジェクト3dが有効状態か
		if (!sprite->IsEnabled()){
			continue;
		}

		//activeSpritesを追加
		activeSprites_.push_back(sprite);
	}

	//レンダラーに追加
	for (Sprite* sprite : activeSprites_){
		spriteRenderer_->AddRenderData(sprite->GetRenderData());
	}
}

//描画に有効なDebugDrawを集める
void RenderSystem::CollectActiveDebugDraw(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//描画に有効なDebugDrawをリセット
	activeDebugDraws_.clear();

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//DebugDrawを取得
		std::vector<debugDraw::BaseShape*> debugDraws = gameObject->GetComponents<debugDraw::BaseShape>();

		for (debugDraw::BaseShape* debugDraw : debugDraws){
			//オブジェクト3dがNullか
			if (!debugDraw){
				continue;
			}

			//オブジェクト3dが有効状態か
			if (!debugDraw->IsEnabled()){
				continue;
			}

			//activeDebugDrawを追加
			activeDebugDraws_.push_back(debugDraw);
		}
	}

	//レンダラーに追加
	for (debugDraw::BaseShape* debugDraw : activeDebugDraws_){
		debugDrawRenderer_->AddRenderData(debugDraw->GetRenderData());
	}
}

//描画に有効なパーティクルシステムを集める
void RenderSystem::CollectActiveParticleSystems(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//今回描画で使用するカメラ
	Camera* renderCamera = selectCamera_.camera;

	//パーティクルシステムの配列をクリア
	activeParticleSystems_.clear();

	//描画で使用するカメラがなければ
	if (!renderCamera){
		return;
	}

	//カメラのワールド座標を取得
	Vector3 cameraWorldPos = renderCamera->GetWorldPos();

	//カリングの生成
	std::unique_ptr<Culling> culling = Culling::Create(renderCamera);

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//object3dを取得
		ParticleSystem* particleSystem = gameObject->GetComponent<ParticleSystem>();

		//object3dがNullか
		if (!particleSystem){
			continue;
		}

		//object3dが有効状態か
		if (!particleSystem->IsEnabled()){
			continue;
		}

		//モデルが設定されているか
		if (!particleSystem->HasModel()){
			continue;
		}

		//モデルの取得
		Model* model = particleSystem->GetModel();
		if (!model){
			continue;
		}

		//activeParticleSystemsに追加
		activeParticleSystems_.push_back(particleSystem);
	}

	//レンダラーに追加
	for (ParticleSystem* particleSystem : activeParticleSystems_){
		particleRenderer_->AddRenderData(particleSystem->GetRenderData());
	}
}

//描画に有効なカメラを集める
void RenderSystem::CollectActiveCameras(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//描画に有効なCameraをリセット
	activeCameras_.clear();

	//探索開始
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//Cameraを取得
		Camera* camera = gameObject->GetComponent<Camera>();

		//オブジェクト3dがNullか
		if (!camera){
			continue;
		}

		//オブジェクト3dが有効状態か
		if (!camera->IsEnabled()){
			continue;
		}

		//activeCameraを追加
		activeCameras_.push_back(camera);
	}

	//レンダラーに追加
	for (Camera* camera : activeCameras_){
		cameraRenderer_->AddRenderData(camera->GetRenderData());
	}
}

//カメラモードの設定
bool RenderSystem::SetCameraMode(CameraMode cameraMode){
	cameraMode_ = cameraMode;
	return FindSelectCamera(cameraMode_);
}

//描画開始
void RenderSystem::PreDraw(BlendMode blendMode, PipelineType pipelineType){
	//パイプラインのセットを取得
	PipelineSet pipelineSet = pipelineManager_->GetPipelineSet(pipelineType);
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//PSO
	ID3D12PipelineState* pso = pipelineSet.graphicsPipelineStates[static_cast<uint32_t>(blendMode)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
}

//描画開始
void RenderSystem::PreDraw(BlendMode blendMode){
	//パイプラインのセットを取得
	PipelineSet pipelineSet = pipelineManager_->GetPipelineSet(PipelineType::kDebugDraw);
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	//PSO
	ID3D12PipelineState* pso = pipelineSet.graphicsPipelineStates[static_cast<uint32_t>(blendMode)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
}

//オブジェクト3dの描画グループを構築
void RenderSystem::BuildObject3dBatches(){
	//配列をクリア
	object3dBatches_.clear();

	//先頭からアクセス
	for (Object3d* object3d : activeObject3ds_){
		//オブジェクト3dがNullだった場合
		if (!object3d){
			continue;
		}

		//Model*BlendModeを取得
		Model* model = object3d->GetRenderModel();
		//マテリアルインスタンスを取得
		MaterialInstance* materialInstance = object3d->GetMaterialInstance();
		//ブレンドモードを取得
		BlendMode blendMode = object3d->GetBlendMode();

		//モデル、またはマテリアルインスタンスがなければスキップ
		if (!model || !materialInstance){
			continue;
		}

		for (const Object3d::NodeMeshInstance& nodeMesh : object3d->GetNodeMeshInstance()){
			const uint32_t meshIndex = nodeMesh.meshIndex;
			//同じModel*とBlendModeのバッチを探す
			auto batchIt = std::find_if(
				object3dBatches_.begin(),
				object3dBatches_.end(),
				[model, materialInstance, blendMode, meshIndex](const Object3dBatch& batch){
					return batch.model == model &&
						batch.materialInstance == materialInstance &&
						batch.blendMode == blendMode &&
						meshIndex == batch.meshIndex;
				}
			);

			if (batchIt != object3dBatches_.end()){
				//見つかった場合instancesに追加
				batchIt->meshDrawInstance.push_back({ object3d,nodeMesh.nodeMatrix });
			} else{
				//見つからなかった場合新しくバッチを作成
				Object3dBatch newBatch = {
					.model = model,
					.materialInstance = materialInstance,
					.blendMode = blendMode,
					.meshIndex = meshIndex
				};
				newBatch.meshDrawInstance.push_back({ object3d,nodeMesh.nodeMatrix });

				//オブジェクト3dのバッチに追加
				object3dBatches_.push_back(std::move(newBatch));
			}
		}
	}
}

//トランスフォーメーションデータの構築
void RenderSystem::BuildTransformationData(){
	//もしカメラがなければ
	if (!selectCamera_.camera){
		return;
	}

	for (Object3dBatch& batch : object3dBatches_){
		//配列クリア
		batch.transformations.clear();
		//サイズを確保
		batch.transformations.reserve(batch.meshDrawInstance.size());
		for (const MeshDrawInstance& meshDraw : batch.meshDrawInstance){
			//Objectのワールド行列を作成
			Matrix4x4 objectWorldMatrix = meshDraw.object3d->MakeRenderWorldMatrix(selectCamera_.camera->GetWorldMatrix());

			//ワールド行列を求める
			Matrix4x4 world = meshDraw.nodeMatrix * objectWorldMatrix;

			//トランスフォーメーション行列
			TransformationMatrix transformation = {};
			transformation.world = world;
			transformation.worldInverseTranspose = world.InverseTranspose();

			//バッチに追加
			batch.transformations.push_back(transformation);
		}
	}
}

//オブジェクト3dのバッチをレンダラーの送る
void RenderSystem::SubmitObject3dBatches(){
	if (!object3dRenderer_){
		return;
	}

	//レンダラーにバッチを送信
	for (const Object3dBatch& batch : object3dBatches_){
		object3dRenderer_->SubmitBatch(batch.model, batch.meshIndex, batch.materialInstance, batch.blendMode, batch.transformations);
	}
}

//セレクトカメラの取得
bool RenderSystem::FindSelectCamera(CameraMode cameraMode){
	selectCamera_ = {};

	for (uint32_t i = 0; i < activeCameras_.size(); i++){
		//カメラ
		Camera* camera = activeCameras_[i];

		//ゲームオブジェクトを取得
		GameObject* gameObject = camera->GetOwner();
		//カメラがないまたはゲームオブジェクトが無い場合
		if (!camera || !gameObject){
			continue;
		}

		//デバッグカメラを取得
		DebugCameraController* debugCamera = camera->GetOwner()->GetComponent<DebugCameraController>();
		if (debugCamera){
			if (cameraMode == CameraMode::kDebug){
				selectCamera_.camera = camera;
				selectCamera_.index = i;
				return true;
			}
		}

		//メインカメラの場合
		if (cameraMode == CameraMode::kMain){
			if (!debugCamera){
				selectCamera_.camera = camera;
				selectCamera_.index = i;
				return true;
			}
		}
	}

	return false;
}
