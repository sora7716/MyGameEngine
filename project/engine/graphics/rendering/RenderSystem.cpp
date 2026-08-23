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
#include "SkyBoxRenderer.h"
#include "DebugDrawRenderer.h"
#include "ParticleRenderer.h"
#include <algorithm>

//コンストラクタ
RenderSystem::RenderSystem(){
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
	//Object3dのレンダラー
	object3dRenderer_ = Object3dRenderer::Create(directXBase, srvManager, textureManager);
	//スカイボックスのレンダラー
	skyBoxRenderer_ = SkyBoxRenderer::Create(directXBase, textureManager);
	//デバッグ描画のレンダラー
	debugDrawRenderer_ = DebugDrawRenderer::Create(directXBase);
	//パーティクルのレンダラー
	particleRenderer_ = ParticleRenderer::Create(directXBase, srvManager, textureManager);
}

//描画
void RenderSystem::Draw(){
	//Object3d
	for (uint32_t i = 0; i < object3dRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(object3dRenderer_->GetBlendMode(i), PipelineType::kObject3d);
		//ライティングの設定
		lightingManager_->DrawSetting();
		//3dオブジェクトの描画
		object3dRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	object3dRenderer_->Reset();

	//Particle
	for (uint32_t i = 0; i < particleRenderer_->GetRenderDataSize(); i++){
		//描画の開始
		PreDraw(particleRenderer_->GetBlendMode(i), PipelineType::kParticle);
		//Particleの描画
		particleRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	particleRenderer_->Reset();

	//デバッグ描画
	for (uint32_t i = 0; i < debugDrawRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(debugDrawRenderer_->GetBlendMode(i));
		//デバッグ描画の描画
		debugDrawRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	debugDrawRenderer_->Reset();

	//スカイボックス
	for (uint32_t i = 0; i < skyBoxRenderer_->GetRenderDataSize(); i++){
		//描画開始
		PreDraw(skyBoxRenderer_->GetBlendMode(i), PipelineType::kSkyBox);
		//スカイボックスの描画
		skyBoxRenderer_->Draw(i);
	}
	//描画オブジェクトのリセット
	skyBoxRenderer_->Reset();

}

//Object3dを集める
void RenderSystem::CollectObject3ds(const std::vector<std::unique_ptr<GameObject>>& gameObjects, Camera* renderCamera){
	//今回描画で使用するカメラ
	renderCamera_ = renderCamera;

	//object3dsをクリア
	object3ds_.clear();

	//描画で使用するカメラがなければ
	if (!renderCamera_){
		object3dBatches_.clear();
		return;
	}

	//カリングの生成
	std::unique_ptr<Culling> culling = Culling::Create(renderCamera);

	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullじゃないか
		if (!gameObject){
			continue;
		}

		//ゲームオブジェクトが有効状態か
		if (!gameObject->IsActive()){
			continue;
		}

		//オブジェクト3dを取得
		Object3d* object3d = gameObject->GetComponent<Object3d>();

		//オブジェクト3dがNullか
		if (!object3d){
			continue;
		}

		//オブジェクト3dが有効状態か
		if (!object3d->IsEnabled()){
			continue;
		}

		//モデルが設定されているか
		if (!object3d->HasModel()){
			continue;
		}

		//モデルを取得
		Model* model = object3d->GetModel();

		//モデルがなければ
		if (!model){
			continue;
		}

		//視錐台カリング
		bool isVisible = false;
		const Matrix4x4 renderWorld = object3d->MakeRenderWorldMatrix(renderCamera_->GetWorldMatrix());
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
		object3ds_.push_back(object3d);
	}

	//収集したObject3dをグループ分け
	BuildObject3dBatches();
	//トランスフォーメーションデータの構築
	BuildTransformationData();
	//作成したバッチをレンダラーに送信
	SubmitObject3dBatches();
}

//Object3dのレンダラーの取得
Object3dRenderer* RenderSystem::GetObject3dRenderer(){
	return object3dRenderer_.get();
}

//SkyBoxのレンダラーの取得
SkyBoxRenderer* RenderSystem::GetSkyBoxRenderer(){
	return skyBoxRenderer_.get();
}

//デバッグ描画のレンダラーの取得
DebugDrawRenderer* RenderSystem::GetDebugDrawRenderer(){
	return debugDrawRenderer_.get();
}

//パーティクルのレンダラーの取得
ParticleRenderer* RenderSystem::GetParticleRenderer(){
	return particleRenderer_.get();
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
	for (Object3d* object3d : object3ds_){
		//オブジェクト3dがNullだった場合
		if (!object3d){
			continue;
		}

		//Model*BlendModeを取得
		Model* model = object3d->GetModel();
		//マテリアルインスタンスを取得
		MaterialInstance* materialInstance = object3d->GetMaterialInstance();
		//ブレンドモードを取得
		BlendMode blendMode = object3d->GetBlendMode();

		//モデル、またはマテリアルインスタンスがなければスキップ
		if (!model || !materialInstance){
			continue;
		}

		//同じModel*とBlendModeのバッチを探す
		auto batchIt = std::find_if(
			object3dBatches_.begin(),
			object3dBatches_.end(),
			[model, materialInstance, blendMode](const Object3dBatch& batch){
				return batch.model == model &&
					batch.materialInstance == materialInstance &&
					batch.blendMode == blendMode;
			}
		);

		if (batchIt != object3dBatches_.end()){
			//見つかった場合instancesに追加
			batchIt->instances.push_back(object3d);
		} else{
			//見つからなかった場合新しくバッチを作成
			Object3dBatch newBatch = {
				.model = model,
				.materialInstance = materialInstance,
				.blendMode = blendMode,
			};
			newBatch.instances.push_back(object3d);

			//オブジェクト3dのバッチに追加
			object3dBatches_.push_back(std::move(newBatch));
		}
	}
}

//トランスフォーメーションデータの構築
void RenderSystem::BuildTransformationData(){
	//もしカメラがなければ
	if (!renderCamera_){
		return;
	}

	for (Object3dBatch& batch : object3dBatches_){
		//配列クリア
		batch.transformations.clear();
		//サイズを確保
		batch.transformations.reserve(batch.instances.size());
		for (Object3d* object3d : batch.instances){
			//今回の描画カメラに対応したワールド行列を作成
			Matrix4x4 world = object3d->MakeRenderWorldMatrix(renderCamera_->GetWorldMatrix());

			//トランスフォーメーション行列
			TransformationMatrix transformation = {};
			transformation.world = world;
			transformation.wvp = world * renderCamera_->GetViewProjectionMatrix();
			transformation.worldInverseTranspose = world.InverseTranspose();

			//バッチに追加
			batch.transformations.push_back(transformation);
		}
	}
}

//オブジェクト3dのバッチをレンダラーの送る
void RenderSystem::SubmitObject3dBatches(){
	if (!object3dRenderer_ || !renderCamera_){
		return;
	}

	//レンダラーにバッチを送信
	for (const Object3dBatch& batch : object3dBatches_){
		object3dRenderer_->SubmitBatch(batch.model, batch.materialInstance, batch.blendMode, batch.transformations, renderCamera_);
	}
}
