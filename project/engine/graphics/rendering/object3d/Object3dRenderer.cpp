#include "Object3dRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Logger.h"
#include "Mesh.h"
#include "Model.h"
#include "MaterialInstance.h"
#include <algorithm>
#include <cassert>

//生成
std::unique_ptr<Object3dRenderer> Object3dRenderer::Create(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, uint32_t maxInstance){
	std::unique_ptr<Object3dRenderer>instance = std::make_unique<Object3dRenderer>();
	instance->Initialize(directXBase, srvManager, textureManager, maxInstance);
	return instance;
}

//コンストラクタ
Object3dRenderer::Object3dRenderer(){
}

//デストラクタ
Object3dRenderer::~Object3dRenderer(){
}

//初期化
void Object3dRenderer::Initialize(DirectXBase* directXBase, SRVManager* srvManager, TextureManager* textureManager, uint32_t maxInstance){
	//DirectXの基盤部分のNullチェック
	assert(directXBase);
	directXBase_ = directXBase;
	//SRVの管理のNullチェック
	assert(srvManager);
	srvManager_ = srvManager;
	//Textureの管理のNullチェック
	assert(textureManager);
	textureManager_ = textureManager;

	//インスタンスの最大値の記録
	maxInstanceCount_ = maxInstance;


	//描画データの初期化
	renderDatas_.reserve(maxInstanceCount_);
	renderDatas_.clear();
}

//オブジェクトを登録
Object3dRenderHandle Object3dRenderer::RegisterObject(uint32_t lodCount, uint32_t maxInstance){

	assert(lodCount > 0);
	assert(maxInstance > 0);

	const Object3dRenderHandle handle = static_cast<Object3dRenderHandle>(objectResources_.size());

	//Object3d一つ分を追加
	objectResources_.emplace_back();

	Object3dGpuResource& objectResource = objectResources_.back();

	//LOD数分のResourceを用意
	objectResource.lodResources.resize(lodCount);

	for (LODGpuResource& lodResource : objectResource.lodResources){
		//このLODに格納できる最大行列数
		lodResource.capacity = maxInstance;

		//TransformationResourceの作成
		CreateTransformationMatrixResource(lodResource);

		//StructuredBufferの作成(Transformation用)
		CreateStructuredBufferForWvp(lodResource);
	}

	return handle;
}

//リセット
void Object3dRenderer::Reset(){
	//描画データをリセット
	renderDatas_.clear();
}

//描画
void Object3dRenderer::Draw(uint32_t instanceIndex, Camera* renderCamera){

	//カメラ
	renderCamera->DrawSetting(4);

	for (uint32_t lodIndex = 0; lodIndex < renderDatas_[instanceIndex].lodRenderData.modelRendererDatas.size(); lodIndex++){
		//LODモデルが存在してなかったら
		if (renderDatas_[instanceIndex].lodRenderData.modelRendererDatas.empty()){
			continue;
		}

		const Object3dGpuResource& objectResource = objectResources_[renderDatas_[instanceIndex].renderHandle];

		const LODGpuResource& lodResource = objectResource.lodResources[lodIndex];

		//モデルの描画に必要なデータ
		ModelRenderData modelRenderData = renderDatas_[instanceIndex].lodRenderData.modelRendererDatas[lodIndex];
		//WVPのSRVIndex
		uint32_t wvpSrvIndex = lodResource.srvIndex;
		//描画カウント
		uint32_t drawCount = renderDatas_[instanceIndex].lodRenderData.matrixCounts[lodIndex];

		//LOD描画カウントが0だったら
		if (drawCount == 0){
			continue;
		}

		//LODごとのWVP SRVを設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(wvpSrvIndex));

		//リムライトのCBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(7, modelRenderData.rimLightResource->GetGPUVirtualAddress());
		//メッシュの描画
		for (const MeshRenderData& meshRenderData : modelRenderData.meshRenderDatas){
			uint32_t materialIndex = meshRenderData.meshData.materialIndex;

			//VertexBufferViewの設定
			directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &meshRenderData.vertexBufferView);

			//IndexBufferViewの設定
			directXBase_->GetCommandList()->IASetIndexBuffer(&meshRenderData.indexBufferView);

			//マテリアルCBufferの場所を設定
			directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, modelRenderData.materialResources[materialIndex]->GetGPUVirtualAddress());

			MaterialTexturePaths& materialTexturePath = modelRenderData.modelData.materialTexturePaths[materialIndex];

			//テクスチャをセット
			directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(materialTexturePath.textureFilePath));

			//環境マップのセット
			directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(8, textureManager_->GetSRVHandleGPU(materialTexturePath.environmentMap));

			//オブジェクト数が0より大きければ
			if (drawCount > 0){
				//メッシュが空じゃなければ
				if (!meshRenderData.meshData.indices.empty()){
					//描画
					directXBase_->GetCommandList()->DrawIndexedInstanced(UINT(meshRenderData.meshData.indices.size()), drawCount, 0, 0, 0);
				}
			}
		}
	}
}

//バッチを受け取る関数
void Object3dRenderer::SubmitBatch(Model* model, MaterialInstance* materialInstance, BlendMode blendMode, const std::vector<TransformationMatrix>& transformations){
	if (!model || !materialInstance){
		return;
	} else if (transformations.empty()){
		return;
	}

	assert(transformations.size() <= maxInstanceCount_);

	//現在のバッチのポインタ
	BatchResource* currentBatchResource = nullptr;
	Object3dRenderHandle handle = kInvalidObject3dRenderHandle;
	//同じModel*とBlendModeのバッチを探す
	auto batchIt = std::find_if(
		batchResources_.begin(),
		batchResources_.end(),
		[model, materialInstance, blendMode](const BatchResource& batchResource){
			return batchResource.model == model &&
				batchResource.materialInstance == materialInstance &&
				batchResource.blendMode == blendMode;
		}
	);

	if (batchIt != batchResources_.end()){
		//見つかった場合
		handle = batchIt->handle;
		//現在のバッチのポインタを保存
		currentBatchResource = &(*batchIt);
	} else{
		//見つからなかった場合
		handle = RegisterObject(1, maxInstanceCount_);

		//新しく生成
		BatchResource newResource = {};
		newResource.model = model;
		newResource.materialInstance = materialInstance;
		newResource.blendMode = blendMode;
		newResource.handle = handle;

		//GPUリソースを生成
		CreateMaterialInstanceResource(newResource);

		//追加
		batchResources_.push_back(std::move(newResource));

		//現在のバッチのポインタを保存
		currentBatchResource = &batchResources_.back();
	}

	//マテリアルインスタンスリソースの更新
	UpdateMaterialInstanceResource(*currentBatchResource);

	//ここからObject3dRenderDataを作る
	Object3dRenderData renderData = {};

	renderData.renderHandle = handle;
	renderData.blendMode = blendMode;

	//Modelからメッシュを含む描画データをコピー
	ModelRenderData modelRenderData = model->GetModelRenderData();
	//マテリアル用のGPUリソースを差し替え　
	modelRenderData.materialResources = currentBatchResource->materialResources;
	//リムライト用のGPUリソースを差し替え
	modelRenderData.rimLightResource = currentBatchResource->rimLightResource;
	//マテリアルインスタンスのスロットを取得
	const std::vector<MaterialInstanceSlot>& slots = materialInstance->GetSlots();
	//モデルの描画データのテクスチャパスの要素数を設定
	modelRenderData.modelData.materialTexturePaths.resize(slots.size());
	//テクスチャパスをコピー
	for (uint32_t i = 0; i < static_cast<uint32_t>(slots.size()); i++){
		modelRenderData.modelData.materialTexturePaths[i] = slots[i].texturePaths;
	}
	//描画データに追加
	renderData.lodRenderData.modelRendererDatas.push_back(std::move(modelRenderData));

	//LOD0の描画数
	renderData.lodRenderData.transformationData.push_back(transformations);

	//LOD0の行列配列数
	renderData.lodRenderData.matrixCounts.push_back(static_cast<uint32_t>(transformations.size()));

	//完成した描画データをGPU側に渡す
	AddRenderData(renderData);
}

//描画データの追加
void Object3dRenderer::AddRenderData(const Object3dRenderData& renderData){
	//ハンドルが有効か
	assert(renderData.renderHandle != kInvalidObject3dRenderHandle);

	//ハンドルが範囲内か
	assert(renderData.renderHandle < objectResources_.size());

	//対応リソースの取得
	Object3dGpuResource& objectResource = objectResources_[renderData.renderHandle];

	//LODごとのTransformationData
	const std::vector<std::vector<TransformationMatrix>>& transformationData = renderData.lodRenderData.transformationData;

	//LODごとの描画数
	const std::vector<uint32_t>drawCounts = renderData.lodRenderData.matrixCounts;

	//CPUとGPUのLOD数を比べて一致しているか
	assert(transformationData.size() == objectResource.lodResources.size());

	//描画数もGPUと一致しているか
	assert(drawCounts.size() == objectResource.lodResources.size());

	for (uint32_t lodIndex = 0; lodIndex < objectResource.lodResources.size(); lodIndex++){
		LODGpuResource& lodResource = objectResource.lodResources[lodIndex];

		const uint32_t drawCount = drawCounts[lodIndex];

		assert(drawCount <= lodResource.capacity);

		std::copy_n(transformationData[lodIndex].data(), drawCount, lodResource.wvpData);
	}

	//レンダーデータの追加
	renderDatas_.push_back(renderData);
}

//ブレンドモードの取得
BlendMode Object3dRenderer::GetBlendMode(uint32_t instanceIndex){
	return renderDatas_[instanceIndex].blendMode;
}

//描画データの配列のサイズの取得
uint32_t Object3dRenderer::GetRenderDataSize(){
	return static_cast<uint32_t>(renderDatas_.size());
}

//座標変換行列リソースの生成
void Object3dRenderer::CreateTransformationMatrixResource(LODGpuResource& lodGpuResource){
	HRESULT result = S_FALSE;
	// 配列サイズで確保
	lodGpuResource.wvpResource = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * lodGpuResource.capacity);
	//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//書き込むためのアドレス
	result = lodGpuResource.wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&lodGpuResource.wvpData));
	assert(SUCCEEDED(result));
	//単位行列を書き込んでおく
	for (uint32_t i = 0; i < lodGpuResource.capacity; i++){
		lodGpuResource.wvpData[i].wvp = Matrix4x4::Identity4x4();
		lodGpuResource.wvpData[i].world = Matrix4x4::Identity4x4();
		lodGpuResource.wvpData[i].worldInverseTranspose = Matrix4x4::Identity4x4();
	}
}

//座標変換行列リソースのストラクチャバッファの生成
void Object3dRenderer::CreateStructuredBufferForWvp(LODGpuResource& lodGpuResource){
	//ストラクチャバッファを生成
	lodGpuResource.srvIndex = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreateSRVForStructuredBuffer(
		lodGpuResource.srvIndex,
		lodGpuResource.wvpResource.Get(),
		lodGpuResource.capacity,
		sizeof(TransformationMatrix)
	);
}

//MaterialInstance用のGPUリソースを生成
void Object3dRenderer::CreateMaterialInstanceResource(BatchResource& batchResource){
	//マテリアルインスタンスがなければ
	if (!batchResource.materialInstance){
		return;
	}

	//マテリアルインスタンスのスロットを取得
	const std::vector<MaterialInstanceSlot>& slots = batchResource.materialInstance->GetSlots();
	batchResource.materialResources.resize(slots.size());
	batchResource.materialPtrs.resize(slots.size());

	//マテリアルの生成
	for (uint32_t i = 0; i < static_cast<uint32_t>(slots.size()); i++){
		batchResource.materialResources[i] = directXBase_->CreateBufferResource(sizeof(Material));
		batchResource.materialResources[i]->Map(0, nullptr, reinterpret_cast<void**>(&batchResource.materialPtrs[i]));
		*batchResource.materialPtrs[i] = slots[i].material;
	}

	//リムライトの生成
	batchResource.rimLightResource = directXBase_->CreateBufferResource(sizeof(RimLight));
	batchResource.rimLightResource->Map(0, nullptr, reinterpret_cast<void**>(&batchResource.rimLightPtr));
	*batchResource.rimLightPtr = batchResource.materialInstance->GetRimLight();

	//マテリアルが変更されたかを判断する変数をコピー
	batchResource.uploadedRevision = batchResource.materialInstance->GetRevision();
}

//マテリアルインスタンスの更新
void Object3dRenderer::UpdateMaterialInstanceResource(BatchResource& batchResource){
	//マテリアルインスタンスがなければ
	if (!batchResource.materialInstance){
		return;
	}

	//変更バージョン
	uint64_t revision = batchResource.materialInstance->GetRevision();

	//バージョンを確認
	if (batchResource.uploadedRevision == revision){
		//変更がなかった場合
		return;
	}

	//変更があった場合
	//マテリアルを変更
	const std::vector<MaterialInstanceSlot>& slots = batchResource.materialInstance->GetSlots();
	for (uint32_t i = 0; i < static_cast<uint32_t>(slots.size()); i++){
		*batchResource.materialPtrs[i] = slots[i].material;
	}
	//リムライトの変更
	*batchResource.rimLightPtr = batchResource.materialInstance->GetRimLight();

	//バージョンを更新
	batchResource.uploadedRevision = revision;
}
