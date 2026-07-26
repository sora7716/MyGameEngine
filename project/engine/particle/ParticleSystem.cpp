#define NOMINMAX
#include <algorithm>
#include <string>
#include "ParticleSystem.h"
#include "DirectXBase.h"
#include "ParticleCommon.h"
#include "Camera.h"
#include "TextureManager.h"
#include "SRVManager.h"
#include "Model.h"
#include "Mesh.h"
#include "PrimitiveMeshFactory.h"

//コンストラクタ
ParticleSystem::ParticleSystem() {
}

//デストラクタ
ParticleSystem::~ParticleSystem() {
}

//初期化
void ParticleSystem::Initialize(ParticleCommon* particleCommon, Camera* renderCamera, const std::string& textureName) {
	//パーティクルの共通部分
	particleCommon_ = particleCommon;

	//DirectXの基盤部分を記録する
	directXBase_ = particleCommon_->GetDirectXBase();

	//エミッター
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize(particleCommon_, renderCamera);

	//メッシュデータを作成
	modelData_.meshDatas.reserve(1);
	modelData_.meshDatas.push_back(primitiveMeshFactory::CreatePlane());
	modelData_.materialTexturePaths.resize(1);

	//メッシュの生成
	meshes_.reserve(modelData_.meshDatas.size());
	for (MeshData& meshData : modelData_.meshDatas) {
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}

	//メッシュの設定
	emitter_->SetMeshes(meshes_);

	//ワールドトランスフォームのリソースの生成
	CreateWorldTransformResource();

	for (uint32_t i = 0; i < meshes_.size(); i++) {
		//テクスチャファイルの記録
		modelData_.materialTexturePaths[i].textureFilePath = "engine/resources/textures/" + textureName;
		//テクスチャの読み込み
		particleCommon_->GetTextureManager()->AddTexture(modelData_.materialTexturePaths[i].textureFilePath);
	}

	//マテリアルリソースの生成
	CreateMaterialResources();

	//ストラクチャバッファの生成
	CreateStructuredBuffer();
}

//更新
void ParticleSystem::Update() {
	emitter_->Update(instancingData_);
}

//描画
void ParticleSystem::Draw() {
	//描画準備
	particleCommon_->DrawSetting();
	//エミッター
	emitter_->DrawSetting();
	//PSOの設定
	auto pso = particleCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, particleCommon_->GetSRVManager()->GetGPUDescriptorHandle(srvIndex_));
	for (uint32_t i = 0; i < meshes_.size(); i++) {
		//マテリアルインデックス
		uint32_t materialIndex = meshes_[i]->GetMaterialIndex();

		//マテリアルCBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResources_[materialIndex]->GetGPUVirtualAddress());

		//テクスチャパスを適応
		MaterialTexturePaths& materialTexturePath = modelData_.materialTexturePaths[materialIndex];
		//SRVのDescriptorTableの先頭を設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, particleCommon_->GetTextureManager()->GetSRVHandleGPU(materialTexturePath.textureFilePath));
		//メッシュの描画
		meshes_[i]->Draw(emitter_->GetNumInstance());
	}
}

//カメラの設定
void ParticleSystem::SetGameCamera(Camera* camera) {
	emitter_->SetGameCamera(camera);
}

//描画カメラの設定
void ParticleSystem::SetRenderCamera(Camera* camera) {
	emitter_->SetRenderCamera(camera);
}

//ブレンドモードの設定
void ParticleSystem::SetBlendMode(BlendMode blendMode) {
	blendMode_ = blendMode;
}

//エミッター位置の設定
void ParticleSystem::SetEmitterPosition(const Vector3& position) {
	emitter_->SetEmitterPosition(position);
}

//パーティクルの数の設定
void ParticleSystem::SetParticleCount(uint32_t cont) {
	emitter_->SetParticleCount(cont);
}

//発生範囲の設定
void ParticleSystem::SetEmitRange(float range) {
	emitter_->SetEmitRange(range);
}

//加速度が起こるフィールドの設定
void ParticleSystem::SetAccelerationField(const AccelerationField& field) {
	emitter_->SetAccelerationField(field);
}

//パーティクルの発生感覚[秒]の設定
void ParticleSystem::SetFrequency(float frequency) {
	emitter_->SetFrequency(frequency);
}

//モデルデータの設定
void ParticleSystem::SetModelData(const ModelData& modelData) {
	//モデルデータを記録
	modelData_ = modelData;
	//メッシュデータをクリア
	meshes_.clear();
	//メモリのサイズを確保(要素数は増えない)
	meshes_.reserve(modelData_.meshDatas.size());
	//メッシュを生成
	for (MeshData& meshData : modelData_.meshDatas) {
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}

	//マテリアルリソースを生成
	CreateMaterialResources();

	//モデルのテクスチャの適応
	for (std::shared_ptr<Mesh>& mesh : meshes_) {
		ApplyModelTexture(mesh->GetMaterialIndex());
	}

	//メッシュの設定
	emitter_->SetMeshes(meshes_);

}

//テクスチャの設定
void ParticleSystem::SetTexture(uint32_t meshIndex, const std::string& imageFileName) {
	//マテリアルの検索キーを取得
	uint32_t materialIndex = meshes_[meshIndex]->GetMaterialIndex();
	modelData_.materialTexturePaths[materialIndex].textureFilePath = "engine/resources/textures/" + imageFileName;
	particleCommon_->GetTextureManager()->AddTexture(modelData_.materialTexturePaths[materialIndex].textureFilePath);
}

//マテリアルリソースの生成
void ParticleSystem::CreateMaterialResources() {
	//マテリアルリソースとポインタのサイズ設定
	materialResources_.resize(modelData_.materialTexturePaths.size());
	materialPtrs_.resize(modelData_.materialTexturePaths.size());
	for (uint32_t i = 0; i < modelData_.materialTexturePaths.size(); i++) {
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

//ワールドトランスフォームのリソースの生成
void ParticleSystem::CreateWorldTransformResource() {
	//座標変換行列リソースを作成する	
	instancingResource_ = directXBase_->CreateBufferResource(sizeof(ParticleForGPU) * ParticleEmitter::kNumMaxInstance);
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	for (uint32_t i = 0; i < ParticleEmitter::kNumMaxInstance; i++) {
		//単位行列を書き込んでおく
		instancingData_[i].WVP = Matrix4x4::Identity4x4();
		instancingData_[i].world = Matrix4x4::Identity4x4();
		instancingData_[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // 初期色を白に設定
	}
}

//ストラクチャバッファの生成
void ParticleSystem::CreateStructuredBuffer() {
	//ストラクチャバッファを生成
	srvIndex_ = particleCommon_->GetSRVManager()->Allocate() + TextureManager::kSRVIndexTop;
	particleCommon_->GetSRVManager()->CreateSRVForStructuredBuffer(
		srvIndex_,
		instancingResource_.Get(),
		ParticleEmitter::kNumMaxInstance,
		sizeof(ParticleForGPU)
	);
}

//モデルのテクスチャを適応
void ParticleSystem::ApplyModelTexture(uint32_t materialIndex) {
	//テクスチャの読み込み
	particleCommon_->GetTextureManager()->AddTexture(modelData_.materialTexturePaths[materialIndex].textureFilePath);
}