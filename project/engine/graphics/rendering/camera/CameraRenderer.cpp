#include "CameraRenderer.h"
#include "DirectXBase.h"

//生成
std::unique_ptr<CameraRenderer> CameraRenderer::Create(ConstructorKey key, DirectXBase* directXBase){
	std::unique_ptr<CameraRenderer>instance = std::make_unique<CameraRenderer>(key);
	instance->Initialize(directXBase);
	return instance;
}

//コンストラクタ
CameraRenderer::CameraRenderer(ConstructorKey){
}

//デストラクタ
CameraRenderer::~CameraRenderer(){
}

//初期化
void CameraRenderer::Initialize(DirectXBase* directXBase){
	//DirectXの基盤部分を記録
	assert(directXBase);
	directXBase_ = directXBase;
}

//描画
void CameraRenderer::Bind(uint32_t instanceIndex, uint32_t parameterIndex){
	//カメラCBufferの場所を設定
	directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(parameterIndex, gpuResources_[instanceIndex].cameraResource->GetGPUVirtualAddress());
}

//リセット
void CameraRenderer::Reset(){
	renderDatas_.clear();
}

//描画データの追加
void CameraRenderer::AddRenderData(const CameraRenderData& renderData){
	renderDatas_.push_back(renderData);
	//インスタンスの検索キー
	const uint32_t instanceIndex = static_cast<uint32_t>(renderDatas_.size() - 1);

	//GPUリソースを追加するか
	if (static_cast<uint32_t>(gpuResources_.size()) <= instanceIndex){
		//リソースを生成
		GpuResource& newGpuResource = gpuResources_.emplace_back();
		//カメラリソースの生成
		CreateCameraResource(newGpuResource);
	}

	GpuResource& gpuResource = gpuResources_[instanceIndex];
	gpuResource.cameraForGPU->worldPosition = renderData.worldPosition;
	gpuResource.cameraForGPU->viewProjection = renderData.viewProjection;
}

//カメラリソースの生成
void CameraRenderer::CreateCameraResource(GpuResource& gpuResource){
	//光源のリソースを作成
	gpuResource.cameraResource = directXBase_->CreateBufferResource(sizeof(CameraForGPU));
	//光源データの書きこみ
	gpuResource.cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&gpuResource.cameraForGPU));
	gpuResource.cameraForGPU->worldPosition = {};
	gpuResource.cameraForGPU->viewProjection = Matrix4x4::Identity4x4();
}
