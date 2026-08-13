#define NOMINMAX
#include <algorithm>
#include <string>
#include "ParticleSystem.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "TextureManager.h"
#include "SRVManager.h"
#include "Model.h"
#include "Mesh.h"
#include "PrimitiveMeshFactory.h"
#include "ParticleEmitter.h"
#include "PipelineManager.h"
#include "ParticleRenderer.h"

//コンストラクタ
ParticleSystem::ParticleSystem(){
}

//デストラクタ
ParticleSystem::~ParticleSystem(){
}

//初期化
void ParticleSystem::Initialize(DirectXBase* directXBase, SRVManager* srvManager, PipelineManager* pipelineManager, Camera* renderCamera, const std::string& textureName){
	//DirectXの基盤部分を記録する
	assert(directXBase);
	directXBase_ = directXBase;
	//SRVの管理の記録
	assert(srvManager);
	srvManager_ = srvManager;

	//パイプラインの管理
	assert(pipelineManager);
	pipelineManager_ = pipelineManager;
	pipelineSet_ = pipelineManager_->GetPipelineSet(PipelineType::kParticle);

	//エミッター
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize(renderCamera);

	//描画用のカメラの記録
	SetRenderCamera(renderCamera);

	//メッシュデータを作成
	modelData_.meshDatas.reserve(1);
	modelData_.meshDatas.push_back(primitiveMeshFactory::CreatePlane());
	modelData_.materialTexturePaths.resize(1);

	//メッシュの生成
	meshes_.reserve(modelData_.meshDatas.size());
	for (MeshData& meshData : modelData_.meshDatas){
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}

	//メッシュの設定
	emitter_->SetMeshes(meshes_);

	for (uint32_t i = 0; i < meshes_.size(); i++){
		//テクスチャファイルの記録
		modelData_.materialTexturePaths[i].textureFilePath = "engine/resources/textures/" + textureName;
	}

	//マテリアルリソースの生成
	CreateMaterialResources();
}

//更新
void ParticleSystem::Update(){
	emitter_->Update(instancingData_);

	//描画データをまとめる
	renderData_.indexBufferView = indexBufferView_;
	renderData_.vertexBufferView = vertexBufferView_;
	renderData_.instanceData = instancingData_;
	renderData_.blendMode = blendMode_;
	renderData_.imageTexturePaths.resize(meshes_.size());
	for (uint32_t i = 0; i < meshes_.size(); i++){
		renderData_.imageTexturePaths[i] = modelData_.materialTexturePaths[i].textureFilePath;
	}
	renderData_.materialResources = materialResources_;
	renderData_.meshes = meshes_;
	renderData_.srvIndex = srvIndex_;
	renderData_.numInstance = emitter_->GetNumInstance();
}

//レンダラーを登録
void ParticleSystem::RegisterToRenderer(ParticleRenderer* renderer){
	assert(renderer);
	//まだ登録されてない事の確認
	assert(renderHandle_ == kInvalidParticleRenderHandle);

	renderHandle_ = renderer->RegisterParticle(ParticleEmitter::kNumMaxInstance);

	renderData_.renderHandle = renderHandle_;
}

void ParticleSystem::DrawSetting(){
	emitter_->UpdateWorldMatrix(instancingData_);
}

//カメラの設定
void ParticleSystem::SetGameCamera(Camera* camera){
	emitter_->SetGameCamera(camera);
}

//描画カメラの設定
void ParticleSystem::SetRenderCamera(Camera* camera){
	emitter_->SetRenderCamera(camera);
	//カメラの記録
	renderCamera_ = camera;
	//描画データのカメラを設定
	renderData_.renderCamera = renderCamera_;
}

//ブレンドモードの設定
void ParticleSystem::SetBlendMode(BlendMode blendMode){
	blendMode_ = blendMode;
}

//エミッター位置の設定
void ParticleSystem::SetEmitterPosition(const Vector3& position){
	emitter_->SetEmitterPosition(position);
}

//パーティクルの数の設定
void ParticleSystem::SetParticleCount(uint32_t cont){
	emitter_->SetParticleCount(cont);
}

//発生範囲の設定
void ParticleSystem::SetEmitRange(float range){
	emitter_->SetEmitRange(range);
}

//加速度が起こるフィールドの設定
void ParticleSystem::SetAccelerationField(const AccelerationField& field){
	emitter_->SetAccelerationField(field);
}

//パーティクルの発生感覚[秒]の設定
void ParticleSystem::SetFrequency(float frequency){
	emitter_->SetFrequency(frequency);
}

//モデルデータの設定
void ParticleSystem::SetModelData(const ModelData& modelData){
	//モデルデータを記録
	modelData_ = modelData;
	//メッシュデータをクリア
	meshes_.clear();
	//メモリのサイズを確保(要素数は増えない)
	meshes_.reserve(modelData_.meshDatas.size());
	//メッシュを生成
	for (MeshData& meshData : modelData_.meshDatas){
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}

	//マテリアルリソースを生成
	CreateMaterialResources();

	//メッシュの設定
	emitter_->SetMeshes(meshes_);
}

//テクスチャの設定
void ParticleSystem::SetTexture(uint32_t meshIndex, const std::string& imageFileName){
	//マテリアルの検索キーを取得
	uint32_t materialIndex = meshes_[meshIndex]->GetMaterialIndex();
	modelData_.materialTexturePaths[materialIndex].textureFilePath = "engine/resources/textures/" + imageFileName;
}

//描画データの取得
const ParticleRenderData& ParticleSystem::GetRenderData(){
	// TODO: return ステートメントをここに挿入します
	return renderData_;
}

//マテリアルリソースの生成
void ParticleSystem::CreateMaterialResources(){
	//マテリアルリソースとポインタのサイズ設定
	materialResources_.resize(modelData_.materialTexturePaths.size());
	materialPtrs_.resize(modelData_.materialTexturePaths.size());
	for (uint32_t i = 0; i < modelData_.materialTexturePaths.size(); i++){
		//マテリアル用のリソースを作る
		materialResources_[i] = directXBase_->CreateBufferResource(sizeof(Material));
		//書き込むためのアドレスを取得
		materialResources_[i]->Map(0, nullptr, reinterpret_cast<void**>(&materialPtrs_[i]));
		//色を書き込む
		materialPtrs_[i]->color = Vector4::MakeWhiteColor();
		materialPtrs_[i]->enableLighting = true;
		materialPtrs_[i]->uvMatrix = Matrix4x4::Identity4x4();
		materialPtrs_[i]->shininess = 10.0f;
		materialPtrs_[i]->environmentCoefficient = 0.0f;
	}
}