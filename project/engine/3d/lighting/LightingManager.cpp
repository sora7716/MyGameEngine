#define NOMINMAX
#include "LightingManager.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "MathUtility.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "GameObject.h"
#include <algorithm>
#include <cassert>

//生成
std::unique_ptr<LightingManager> LightingManager::Create(ConstructorKey key, DirectXBase* directXBase, SRVManager* srvManager){
	//生成
	std::unique_ptr<LightingManager>instance = std::make_unique<LightingManager>(key);
	//初期化
	instance->Initialize(directXBase, srvManager);

	return instance;
}

//コンストラクタ
LightingManager::LightingManager(ConstructorKey){
}

//デストラクタ
LightingManager::~LightingManager(){
	//SRVの解放
	srvManager_->Free(srvIndexPoint_);
	srvManager_->Free(srvIndexSpot_);
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
}

//描画の設定
void LightingManager::Bind(){
	//平光源CBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	//点光源のStructuredBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvManager_->GetGPUDescriptorHandle(srvIndexPoint_));
	//スポットライトのStructuredBufferを設定
	directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, srvManager_->GetGPUDescriptorHandle(srvIndexSpot_));
}

//有効な平行光源を集める
void LightingManager::CollectActiveDirectionalLight(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//GPUのライトをリセット
	directionalLight_->enable = false;

	//ゲームオブジェクトの配列がなければ
	if (gameObjects.empty()){
		return;
	}

	//各オブジェクトへアクセス
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullなら
		if (!gameObject){
			continue;
		}
		//平行光源が取得できた場合
		DirectionalLight* directionalLight = gameObject->GetComponent<DirectionalLight>();
		//平行光源が存在しない場合
		if (!directionalLight){
			continue;
		}

		//平行光源が有効じゃない場合
		if (!directionalLight->IsEnabled()){
			continue;
		}

		//平行光源のデータに保存
		*directionalLight_ = directionalLight->GetDirectionalLightData();
	}
}

//有効なポイントライトを集める
void LightingManager::CollectActivePointLight(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//GPUのライトをリセット
	for (uint32_t i = 0; i < kMaxLightCount; i++){
		mappedPointLightData_[i].enable = false;
	}
	//有効な点光源の配列をクリア
	pointLights_.clear();

	//ゲームオブジェクトの配列がなければ
	if (gameObjects.empty()){
		return;
	}

	//各オブジェクトへアクセス
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullなら
		if (!gameObject){
			continue;
		}
		//点光源が取得できた場合
		PointLight* pointLight = gameObject->GetComponent<PointLight>();
		//点光源が存在しない場合
		if (!pointLight){
			continue;
		}

		//点光源が有効じゃない場合
		if (!pointLight->IsEnabled()){
			continue;
		}

		//ライトの最大値を超えていた場合
		if (pointLights_.size() >= kMaxLightCount){
			break;
		}

		//有効な点光源を保存
		pointLights_.push_back(pointLight->GetPointLightData());
	}

	//GPUに送信
	std::memcpy(mappedPointLightData_, pointLights_.data(), pointLights_.size() * sizeof(lightData::PointLight));
}

//有効なスポットライトを集める
void LightingManager::CollectActiveSpotLight(const std::vector<std::unique_ptr<GameObject>>& gameObjects){
	//GPUのライトをリセット
	for (uint32_t i = 0; i < kMaxLightCount; i++){
		mappedSpotLightData_[i].enable = false;
	}
	//有効なスポットライトの配列をクリア
	spotLights_.clear();

	//ゲームオブジェクトの配列がなければ
	if (gameObjects.empty()){
		return;
	}

	//各オブジェクトへアクセス
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects){
		//ゲームオブジェクトがNullなら
		if (!gameObject){
			continue;
		}
		//スポットライトが取得できた場合
		SpotLight* spotLight = gameObject->GetComponent<SpotLight>();
		//スポットライトが存在しない場合
		if (!spotLight){
			continue;
		}

		//スポットライトが有効じゃない場合
		if (!spotLight->IsEnabled()){
			continue;
		}

		//ライトの最大値を超えていた場合
		if (spotLights_.size() >= kMaxLightCount){
			break;
		}

		//有効なスポットライトを保存
		spotLights_.push_back(spotLight->GetSpotLightData());
	}

	//GPUに送信
	std::memcpy(mappedSpotLightData_, spotLights_.data(), spotLights_.size() * sizeof(lightData::SpotLight));
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
	directionalLight_->enable = true;
}

//点光源の生成
void LightingManager::CreatePointLight(){
	// 配列サイズで確保
	pointLightResource_ = directXBase_->CreateBufferResource(sizeof(lightData::PointLight) * kMaxLightCount);

	//配列としてMap
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedPointLightData_));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i){
		pointLights_[i].color = { 1,1,1,1 };
		pointLights_[i].position = { 0,-1,0 };
		pointLights_[i].intensity = 10.0f;
		pointLights_[i].distance = 7.0f;
		pointLights_[i].decay = 2.0f;
		pointLights_[i].enable = false;
	}

	//配列の内容をコピー
	std::memcpy(mappedPointLightData_, pointLights_.data(), pointLights_.size() * sizeof(lightData::PointLight));
}

//点光源のストラクチャバッファの生成
void LightingManager::CreateStructuredBufferForPoint(){
	//ストラクチャバッファを生成
	srvIndexPoint_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexPoint_,
		pointLightResource_.Get(),
		kMaxLightCount,
		sizeof(lightData::PointLight)
	);
}

//スポットライトの生成
void LightingManager::CreateSpotLight(){
	// 配列サイズで確保
	spotLightResource_ = directXBase_->CreateBufferResource(sizeof(lightData::SpotLight) * kMaxLightCount);

	//光源データの書きこみ
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedSpotLightData_));

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
		spotLights_[i].enable = false;
	}

	//配列の内容をコピー
	std::memcpy(mappedSpotLightData_, spotLights_.data(), spotLights_.size() * sizeof(lightData::SpotLight));
}

//スポットライトのストラクチャバッファの生成
void LightingManager::CreateStructuredBufferForSpot(){
	//ストラクチャバッファを生成
	srvIndexSpot_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexSpot_,
		spotLightResource_.Get(),
		kMaxLightCount,
		sizeof(lightData::SpotLight)
	);
}
