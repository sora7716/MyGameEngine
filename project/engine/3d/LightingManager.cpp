#define NOMINMAX
#include "LightingManager.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "MathUtility.h"
#include <algorithm>
#include <cassert>

//コンストラクタ
LightingManager::LightingManager(ConstructorKey){
}

//デストラクタ
LightingManager::~LightingManager(){
}

//初期化
void LightingManager::Initialize(DirectXBase* directXBase, SRVManager* srvManager){
	//DirectXの基盤部分の記録
	assert(directXBase);
	directXBase_ = directXBase;
	//SRVManagerの記録
	assert(srvManager);
	srvManager_ = srvManager;

	//ライティング
	//平行光源の生成
	CreateDirectionLight();
	//点光源の生成
	pointLights_.resize(kMaxLightCount);
	CreatePointLight();
	CreateStructuredBufferForPoint();
	//スポットライトの生成
	spotLights_.resize(kMaxLightCount);
	CreateSpotLight();
	CreateStructuredBufferForSpot();
}

//更新
void LightingManager::Update(){
	//directionalLight_->direction.x = std::clamp(directionalLight_->direction.x, -1.0f, 1.0f);
	//directionalLight_->direction.y = std::clamp(directionalLight_->direction.y, -1.0f, 1.0f);
	//directionalLight_->direction.z = std::clamp(directionalLight_->direction.z, -1.0f, 1.0f);
}

//描画の設定
void LightingManager::DrawSetting(){
	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvManager_->GetGPUDescriptorHandle(srvIndexPoint_));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, srvManager_->GetGPUDescriptorHandle(srvIndexSpot_));
}

//平行光源の設定
void LightingManager::SetDirectionalLight(const DirectionalLight& directionalLight){
	*directionalLight_ = directionalLight;
	directionalLight_->direction = directionalLight.direction.Normalize();
	directionalLight_->intensity = std::max(directionalLight.intensity, 0.0f);
}

//平行光源の取得
DirectionalLight* LightingManager::GetDirectionalLight() const{
	// TODO: return ステートメントをここに挿入します
	return directionalLight_;
}

//平行光源の生成
void LightingManager::CreateDirectionLight(){
	//光源のリソースを作成
	directionalLightResource_ = directXBase_->CreateBufferResource(sizeof(DirectionalLight));
	//光源データの書きこみ
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	directionalLight_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_->direction = { 0.0f,-1.0f,0.0f };
	directionalLight_->intensity = 1.0f;
	directionalLight_->isLambert = false;
	directionalLight_->isBlinnPhong = true;
	directionalLight_->enableDirectionalLighting = true;
}

//点光源の生成
void LightingManager::CreatePointLight(){
	// 配列サイズで確保
	pointLightResource_ = directXBase_->CreateBufferResource(sizeof(PointLight) * kMaxLightCount);

	//点光源
	PointLight* pointLightData = nullptr;

	//配列としてMap
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i){
		pointLights_[i].color = { 1,1,1,1 };
		pointLights_[i].position = { 0,-1,0 };
		pointLights_[i].intensity = 10.0f;
		pointLights_[i].distance = 7.0f;
		pointLights_[i].decay = 2.0f;
		pointLights_[i].isBlinnPhong = true;
		pointLights_[i].enablePointLighting = false;
	}

	//配列の内容をコピー
	std::memcpy(pointLightData, pointLights_.data(), pointLights_.size() * sizeof(PointLight));
}

//点光源のストラクチャバッファの生成
void LightingManager::CreateStructuredBufferForPoint(){
	//ストラクチャバッファを生成
	srvIndexPoint_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexPoint_,
		pointLightResource_.Get(),
		kMaxLightCount,
		sizeof(PointLight)
	);
}

//スポットライトの生成
void LightingManager::CreateSpotLight(){
	// 配列サイズで確保
	spotLightResource_ = directXBase_->CreateBufferResource(sizeof(SpotLight) * kMaxLightCount);

	//スポットライト
	SpotLight* spotLightData = nullptr;
	//光源データの書きこみ
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i){
		spotLights_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLights_[i].position = { 2.0f,1.25f,0.0f };
		spotLights_[i].distance = 7.0f;
		spotLights_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
		spotLights_[i].intensity = 4.0f;
		spotLights_[i].decay = 2.0f;
		spotLights_[i].cosAngle = std::cos(mathUtility::kPi / 3.0f);
		spotLights_[i].cosFalloffStart = 1.0f;
		spotLights_[i].isBlinnPhong = true;
		spotLights_[i].enableSpotLighting = false;
	}

	//配列の内容をコピー
	std::memcpy(spotLightData, spotLights_.data(), spotLights_.size() * sizeof(SpotLight));
}

//スポットライトのストラクチャバッファの生成
void LightingManager::CreateStructuredBufferForSpot(){
	//ストラクチャバッファを生成
	srvIndexSpot_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexSpot_,
		spotLightResource_.Get(),
		kMaxLightCount,
		sizeof(SpotLight)
	);
}
