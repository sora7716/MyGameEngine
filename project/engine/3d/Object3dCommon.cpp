#include "Object3dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "ImGuiManager.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtility.h"
#include "PipelineManager.h"
using namespace Microsoft::WRL;

//デストラクタ
Object3dCommon::~Object3dCommon(){
}

//初期化
void Object3dCommon::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, ModelManager* modelManager, PipelineManager* pipelineManager){
	//DirectXの基盤を受け取る
	directXBase_ = directXBase;
	//SRVの管理を受け取る
	srvManager_ = srvManager;
	//テクスチャの管理を受け取る
	textureManager_ = textureManager;
	//モデルの管理
	modelManager_ = modelManager;
	//パイプラインの管理
	pipelineManager_ = pipelineManager;
	//パイプラインセット
	pipelineSet_ = pipelineManager_->GetPipelineSet(PiplineType::kObject3d);

	//DirectionalLightの初期化
	directionalLightData_.color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData_.direction = { 0.0f,-1.0f,0.0f };
	directionalLightData_.intensity = 1.0f;
	directionalLightData_.isLambert = false;
	directionalLightData_.isBlinnPhong = true;
	directionalLightData_.enableDirectionalLighting = true;

	//PointLightの初期化
	for (int i = 0; i < kMaxLightCount; i++){
		pointLightDataList_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		pointLightDataList_[i].position = {};
		pointLightDataList_[i].intensity = 1.0f;
		pointLightDataList_[i].distance = 7.0f;
		pointLightDataList_[i].decay = 4.0f;
		pointLightDataList_[i].isBlinnPhong = false;
		pointLightDataList_[i].enablePointLighting = false;
	}

	pointLightDataList_[0].position = {};
	pointLightDataList_[0].intensity = 1.5f;
	pointLightDataList_[0].distance = 9.5f;
	pointLightDataList_[0].decay = 15.0f;
	pointLightDataList_[0].isBlinnPhong = true;
	pointLightDataList_[0].enablePointLighting = true;

	////SpotLightの初期化
	//for (int i = 0; i < kMaxLightCount; i++) {
	//	spotLightDataList_[i].color = { 1.0f,1.0f,1.0f,1.0f };
	//	spotLightDataList_[i].position = { 2.0f,1.25f,0.0f };
	//	spotLightDataList_[i].distance = 7.0f;
	//	spotLightDataList_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
	//	spotLightDataList_[i].intensity = 4.0f;
	//	spotLightDataList_[i].decay = 2.0f;
		//spotLightDataList_[i].cosFollOffStart = 1.0f;
	//	spotLightDataList_[i].cosAngle = std::cos(MathUtility::kPi / 3.0f);
	//	spotLightDataList_[i].isBlinnPhong = false;
	//	spotLightDataList_[i].enableSpotLighting = false;
	//}

	//ライティング
	//平行光源の生成
	CreateDirectionLight();
	//点光源の生成
	CreatePointLight();
	CreateStructuredBufferForPoint();
	//スポットライトの生成
	CreateSpotLight();
	spotLightList_.resize(kMaxLightCount);
	CreateStructuredBufferForSpot();
}

//更新
void Object3dCommon::Update(){
	//ライトデータを転送
	*directionalLightPtr_ = directionalLightData_;

	pointLightDataList_[0].position = pointLightPos_;
	for (int32_t i = 0; i < kMaxLightCount; i++){
		pointLightPtr_[i] = pointLightDataList_[i];
	}

	//スポットライトのポインタにデータを転送
	for (int32_t i = 0; i < spotLightList_.size(); i++){
		spotLightPtr_[i] = spotLightList_[i];
	}
}

//共通描画設定
void Object3dCommon::DrawSetting(){
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(pipelineSet_.rootSignature.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//デバッグ
void Object3dCommon::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("Lighting");
	ImGui::ColorEdit4("directional.color", &directionalLightData_.color.x);
	ImGui::DragFloat3("directional.direction", &directionalLightData_.direction.x, 0.1f);
	ImGui::DragFloat("directional.intensity", &directionalLightData_.intensity, 0.1f);
	ImGuiManager::CheckBoxToInt("directional.isLambert", directionalLightData_.isLambert);
	ImGuiManager::CheckBoxToInt("directional.isBlingPhong", directionalLightData_.isBlinnPhong);
	ImGuiManager::CheckBoxToInt("directional.enableDirectionalLight", directionalLightData_.enableDirectionalLighting);

	ImGui::DragFloat("pointLight.decay", &pointLightDataList_[0].decay, 0.1f);
	ImGui::DragFloat("pointLight.distance", &pointLightDataList_[0].distance, 0.1f);
	ImGui::DragFloat("pointLight.intensity", &pointLightDataList_[0].intensity, 0.1f);
	ImGui::End();
#endif // USE_IMGUi
}

//DirectionalLightのリソースのゲッター
ID3D12Resource* Object3dCommon::GetDirectionalLightResource()const{
	return directionalLightResource_.Get();
}

//PointLightのリソースのゲッター
ID3D12Resource* Object3dCommon::GetPointLightResource() const{
	return pointLightResource_.Get();
}

//SpotLightのリソースのゲッター
ID3D12Resource* Object3dCommon::GetSpotLightResource() const{
	return spotLightResource_.Get();
}

//DirectXの基盤のゲッター
DirectXBase* Object3dCommon::GetDirectXBase() const{
	return directXBase_;
}

//SRVマネージャーのゲッター
SRVManager* Object3dCommon::GetSRVManager() const{
	return srvManager_;
}

//テクスチャマネージャーのゲッター
TextureManager* Object3dCommon::GetTextureManager() const{
	return textureManager_;
}

//モデルマネージャーのゲッター
ModelManager* Object3dCommon::GetModelManager() const{
	return modelManager_;
}

//グラフィックパイプラインのゲッター
std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> Object3dCommon::GetGraphicsPipelineStates() const{
	return pipelineSet_.graphicsPipelineStates;
}

// デフォルトカメラのセッター
void Object3dCommon::SetDefaultCamera(Camera* camera){
	defaultCamera_ = camera;
}

// デフォルトカメラのゲッター
Camera* Object3dCommon::GetDefaultCamera() const{
	return defaultCamera_;
}

//SRVインデックスのゲッター(PointLight)
uint32_t Object3dCommon::GetSrvIndexPoint() const{
	return srvIndexPoint_;
}

//SRVインデックスのゲッター(SpotLight)
uint32_t Object3dCommon::GetSrvIndexSpot() const{
	return srvIndexSpot_;
}

//平行光源のゲッター
const DirectionalLight& Object3dCommon::GetDirectionalLight() const{
	// TODO: return ステートメントをここに挿入します
	return directionalLightData_;
}

//点光源のセッター
PointLight* Object3dCommon::GetPointLight(){
	return pointLightDataList_;
}

//スポットライトのゲッター
SpotLight* Object3dCommon::GetSpotLightPtr(){
	return spotLightPtr_;
}

//スポットライトのセッター
void Object3dCommon::SetSpotLightList(uint32_t index, const SpotLight& spotLight){
	spotLightList_[index] = spotLight;
}

//ポイントライトの位置
void Object3dCommon::SetPointLightPos(const Vector3& pointLightPos){
	pointLightPos_ = pointLightPos;
}

//コンストラクタ
Object3dCommon::Object3dCommon(ConstructorKey){
}

//平行光源の生成
void Object3dCommon::CreateDirectionLight(){
	//光源のリソースを作成
	directionalLightResource_ = directXBase_->CreateBufferResource(sizeof(DirectionalLight));
	//光源データの書きこみ
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightPtr_));
	directionalLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightPtr_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightPtr_->intensity = 10.0f;
	directionalLightPtr_->isLambert = false;
	directionalLightPtr_->isBlinnPhong = true;
	directionalLightPtr_->enableDirectionalLighting = true;
}

//点光源の生成
void Object3dCommon::CreatePointLight(){
	// 配列サイズで確保
	pointLightResource_ = directXBase_->CreateBufferResource(sizeof(PointLight) * kMaxLightCount);

	//配列としてMap
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightPtr_));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i){
		pointLightPtr_[i].color = { 1,1,1,1 };
		pointLightPtr_[i].position = { 0,-1,0 };
		pointLightPtr_[i].intensity = 10.0f;
		pointLightPtr_[i].distance = 7.0f;
		pointLightPtr_[i].decay = 2.0f;
		pointLightPtr_[i].isBlinnPhong = true;
		pointLightPtr_[i].enablePointLighting = false;
	}
}

//点光源のストラクチャバッファの生成
void Object3dCommon::CreateStructuredBufferForPoint(){
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
void Object3dCommon::CreateSpotLight(){
	// 配列サイズで確保
	spotLightResource_ = directXBase_->CreateBufferResource(sizeof(SpotLight) * kMaxLightCount);

	//光源データの書きこみ
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightPtr_));

	// とりあえず全部初期化（必要なら0番だけGUI値を入れる）
	for (uint32_t i = 0; i < kMaxLightCount; ++i){
		spotLightPtr_[i].color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightPtr_[i].position = { 2.0f,1.25f,0.0f };
		spotLightPtr_[i].distance = 7.0f;
		spotLightPtr_[i].direction = Vector3({ -1.0f,-1.0f,0.0f }).Normalize();
		spotLightPtr_[i].intensity = 4.0f;
		spotLightPtr_[i].decay = 2.0f;
		spotLightPtr_[i].cosAngle = std::cos(mathUtility::kPi / 3.0f);
		spotLightPtr_[i].cosFalloffStart = 1.0f;
		spotLightPtr_[i].isBlinnPhong = true;
		spotLightPtr_[i].enableSpotLighting = false;
	}
}

//スポットライトのストラクチャバッファの生成
void Object3dCommon::CreateStructuredBufferForSpot(){
	//ストラクチャバッファを生成
	srvIndexSpot_ = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		srvIndexSpot_,
		spotLightResource_.Get(),
		kMaxLightCount,
		sizeof(SpotLight)
	);
}