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

//コンストラクタ
ParticleSystem::ParticleSystem() {
}

//デストラクタ
ParticleSystem::~ParticleSystem() {
}

//初期化
void ParticleSystem::Initialize(ParticleCommon* particleCommon, Camera* renderCamera, const std::string& textureName, Model* model) {
	//パーティクルの共通部分
	particleCommon_ = particleCommon;

	//DirectXの基盤部分を記録する
	directXBase_ = particleCommon_->GetDirectXBase();

	//エミッター
	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize(particleCommon_, renderCamera, model);

	if (model) {
		//メッシュデータの記録
		modelData_ = model->GetModelData();
	} else {
		//メッシュデータを作成
		modelData_.mesheDatas.reserve(1);
		modelData_.mesheDatas.push_back(InitializePlaneModelData());
		modelData_.materialTexturePath.resize(1);
	}

	//メッシュの生成
	meshes_.reserve(modelData_.mesheDatas.size());
	for (MeshData& meshData : modelData_.mesheDatas) {
		std::unique_ptr<Mesh>mesh = std::make_unique<Mesh>();
		mesh->Initialize(directXBase_, meshData);
		meshes_.push_back(std::move(mesh));
	}

	//ワールドトランスフォームのリソースの生成
	CreateWorldTransformResource();

	for (uint32_t i = 0; i < meshes_.size(); i++) {
		//テクスチャファイルの記録
		modelData_.materialTexturePath[i].textureFilePath = "engine/resources/textures/" + textureName;
		//テクスチャの読み込み
		particleCommon_->GetTextureManager()->LoadTexture(modelData_.materialTexturePath[i].textureFilePath);
	}

	//マテリアルリソースの生成
	CreateMaterialResource();

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
	//PSOの設定
	auto pso = particleCommon_->GetGraphicsPipelineStates()[static_cast<int32_t>(blendMode_)].Get();
	//グラフィックスパイプラインをセットするコマンド
	directXBase_->GetCommandList()->SetPipelineState(pso);
	//VertexBufferViewの設定
	directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//IndexBufferViewの設定
	directXBase_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);//IBVを設定
	//マテリアルCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());//material
	//ワールドトランスフォームの描画
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, particleCommon_->GetSRVManager()->GetGPUDescriptorHandle(srvIndex_));
	for (uint32_t i = 0; i < meshes_.size(); i++) {
		//SRVのDescriptorTableの先頭を設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, particleCommon_->GetTextureManager()->GetSRVHandleGPU(modelData_.materialTexturePath[i].textureFilePath));
		//メッシュの描画
		meshes_[i]->Draw(emitter_->GetNumInstance());
	}
}


//デバッグ
void ParticleSystem::Debug() {
	emitter_->Debug();
}

//カメラの設定
void ParticleSystem::SetGameCamera(Camera* camera) {
	emitter_->SetGameCamera(camera);
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

//モデルデータの初期化
MeshData ParticleSystem::InitializePlaneModelData() {
	MeshData meshData = {};
	//サイズ決定
	meshData.vertices.resize(4);
	meshData.indices.resize(6);

	//頂点
	//左上
	meshData.vertices[0] = {
		.position = {-1.0f,1.0f,0.0f,1.0f},
		.texcoord = {0.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
	};
	//右上
	meshData.vertices[1] = {
		.position = {1.0f,1.0f,0.0f,1.0f},
		.texcoord = {1.0f,0.0f},
		.normal = {0.0f,0.0f,1.0f}
	};
	//右下
	meshData.vertices[2] = {
		.position = {1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {1.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
	};
	//左下
	meshData.vertices[3] = {
		.position = {-1.0f,-1.0f,0.0f,1.0f},
		.texcoord = {0.0f,1.0f},
		.normal = {0.0f,0.0f,1.0f}
	};

	//インデックス
	meshData.indices[0] = 0;
	meshData.indices[1] = 1;
	meshData.indices[2] = 2;
	meshData.indices[3] = 0; 
	meshData.indices[4] = 2; 
	meshData.indices[5] = 3;

	return meshData;
}

//マテリアルデータの初期化
void ParticleSystem::InitializeMaterialData() {
	//色を書き込む
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvMatrix = Matrix4x4::Identity4x4();
}

//マテリアルリソースの生成
void ParticleSystem::CreateMaterialResource() {
	//マテリアルリソースを作る
	materialResource_ = directXBase_->CreateBufferResource(sizeof(Material));
	//マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//マテリアルデータの初期値を書き込む
	InitializeMaterialData();
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
		instancingData_[i].World = Matrix4x4::Identity4x4();
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