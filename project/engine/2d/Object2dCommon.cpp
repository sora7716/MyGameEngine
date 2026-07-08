#include "Object2dCommon.h"
#include "DirectXBase.h"
#include "Camera.h"
#include "GraphicsPipeline.h"
#include "Blend.h"
#include <cassert>
using namespace Microsoft::WRL;

//デストラクタ
Object2dCommon::~Object2dCommon() {
	delete blend_;
	delete makeGraphicsPipeline_;
}

//初期化
void Object2dCommon::Initialize(DirectXBase* directXBase, TextureManager* textureManager) {
	//DirectXの基盤を受け取る
	directXBase_ = directXBase;

	//テクスチャマネージャー
	textureManager_ = textureManager;

	//ブレンド
	blend_ = new Blend();
	//グラフィックスパイプラインの生成と初期化
	makeGraphicsPipeline_ = new GraphicsPipeline();
	//シェーダを設定
	makeGraphicsPipeline_->SetVertexShaderFileName(L"Sprite.VS.hlsl");
	makeGraphicsPipeline_->SetPixelShaderFileName(L"Sprite.PS.hlsl");
	//DirectXBaseの記録
	makeGraphicsPipeline_->SetDirectXBase(directXBase);
	//深度バッファ
	makeGraphicsPipeline_->CreateDepthStencilResourceForObject3d();
	//シグネイチャBlobの初期化
	makeGraphicsPipeline_->CreateRootSignatureBlobForSprite();
	//ルートシグネイチャの保存
	makeGraphicsPipeline_->CreateRootSignature();
	//インプットレイアウト
	makeGraphicsPipeline_->InitializeInputLayoutDesc();
	//ラスタライザステート
	makeGraphicsPipeline_->InitializeRasterizerState();
	//頂点シェーダBlob
	makeGraphicsPipeline_->CompileVertexShader();
	//ピクセルシェーダBlob
	makeGraphicsPipeline_->CompilePixelShader();
	//PSO
	for (uint32_t i = 0; i < static_cast<int32_t>(BlendMode::kCountOfBlendMode); i++) {
		//ブレンドステート
		makeGraphicsPipeline_->InitializeBlendState(i);
		//グラフィックスパイプラインの生成
		graphicsPipelineStates_[i] = makeGraphicsPipeline_->CreateGraphicsPipeline();
	}
	//ルートシグネイチャの記録
	rootSignature_ = makeGraphicsPipeline_->GetRootSignature();

}

//共通描画設定
void Object2dCommon::DrawSetting() {
	//ルートシグネイチャをセットするコマンド
	directXBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//プリミティブトポロジーをセットするコマンド
	directXBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//光源の生成
void Object2dCommon::CreateDirectionLight() {
	//光源のリソースを作成
	directionalLightResource_ = directXBase_->CreateBufferResource(sizeof(DirectionalLight));
	//光源データの書きこみ
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightPtr_));
	directionalLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightPtr_->direction = { 0.0f,-1.0f,0.0f };
	directionalLightPtr_->intensity = 1.0f;
}

//点光源の生成
void Object2dCommon::CreatePointLight() {
	//光源のリソースを作成
	pointLightResource_ = directXBase_->CreateBufferResource(sizeof(PointLight));
	//光源データの書きこみ
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightPtr_));
	pointLightPtr_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightPtr_->position = { 0.0f,-1.0f,0.0f };
	pointLightPtr_->intensity = 10.0f;
}

//DirectionalLightのセッター
void Object2dCommon::SetDirectionalLightData(const DirectionalLight& directionalLightData) {
	directionalLightPtr_->color = directionalLightData.color;
	directionalLightPtr_->direction = directionalLightData.direction;
	directionalLightPtr_->intensity = directionalLightData.intensity;
}

//DirectionalLightのリソースのゲッター
ID3D12Resource* Object2dCommon::GetDirectionalLightResource()const {
	return directionalLightResource_.Get();
}

//PointLightのリソースのゲッター
ID3D12Resource* Object2dCommon::GetPointLightResource()const {
	return pointLightResource_.Get();
}

//DirectXの基盤のゲッター
DirectXBase* Object2dCommon::GetDirectXBase() const {
	return directXBase_;
}

//テクスチャマネージャーのゲッター
TextureManager* Object2dCommon::GetTextureManager() const {
	return textureManager_;
}

//グラフィックパイプラインのゲッター
std::array<ComPtr<ID3D12PipelineState>, static_cast<int32_t>(BlendMode::kCountOfBlendMode)> Object2dCommon::GetGraphicsPipelineStates() const {
	return graphicsPipelineStates_;
}

// デフォルトカメラのセッター
void Object2dCommon::SetDefaultCamera(Camera* camera) {
	defaultCamera_ = camera;
}

// デフォルトカメラのゲッター
Camera* Object2dCommon::GetDefaultCamera() const {
	return defaultCamera_;
}

//コンストラクタ
Object2dCommon::Object2dCommon(ConstructorKey) {
}
