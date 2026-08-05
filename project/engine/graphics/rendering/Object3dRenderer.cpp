#include "Object3dRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Logger.h"
#include "Mesh.h"
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

	//TransformationResourceの作成
	CreateTransformationMatrixResource();
	//StructuredBufferの作成(Transformation用)
	CreateStructuredBufferForWvp();
}

//リセット
void Object3dRenderer::Reset(){
	//描画データをリセット
	renderDatas_.clear();
}

//描画
void Object3dRenderer::Draw(){
	for (const Object3dRenderData& renderData : renderDatas_){

		//カメラ
		renderData.renderCamera->DrawSetting(4);

		for (uint32_t lodIndex = 0; lodIndex < renderData.lodRenderData.modelRendererDatas.size(); lodIndex++){
			//LODモデルが存在してなかったら
			if (renderData.lodRenderData.modelRendererDatas.empty()){
				continue;
			}

			//モデルの描画に必要なデータ
			ModelRenderData modelRenderData = renderData.lodRenderData.modelRendererDatas[lodIndex];
			//WVPのSRVIndex
			uint32_t wvpSrvIndex = renderData.lodRenderData.wvpSrvIndices[lodIndex];
			//描画カウント
			uint32_t drawCount = renderData.lodRenderData.drawCounts[lodIndex];

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

				//マテリアルCBufferの場所を設定
				directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(0, modelRenderData.materialResources[materialIndex]->GetGPUVirtualAddress());

				MaterialTexturePaths& materialTexturePath = modelRenderData.modelData.materialTexturePaths[materialIndex];

				//テクスチャをセット
				directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(materialTexturePath.textureFilePath));

				//環境マップのセット
				directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(8, textureManager_->GetSRVHandleGPU(materialTexturePath.environmentMap));
				//VertexBufferViewの設定
				directXBase_->GetCommandList()->IASetVertexBuffers(0, 1, &meshRenderData.vertexBufferView);//VBVを設定
				directXBase_->GetCommandList()->IASetIndexBuffer(&meshRenderData.indexBufferView);//IBVを設定
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
}

//描画データの追加
void Object3dRenderer::AddRenderData(const Object3dRenderData& renderData){
	//インスタンスの最大値
	if (maxInstanceCount_ <= renderDatas_.size()){
		Logger::OutputLog("想定していたインスタンスの最大値を超えて追加しています");
		assert(false);
	}

	//レンダーデータの追加
	renderDatas_.push_back(renderData);
}

//座標変換行列リソースの生成
void Object3dRenderer::CreateTransformationMatrixResource(){
	for (Object3dGpuResource& objectResource : objectResources_){
		for (LODGpuResource& lodResource : objectResource.lodResources){
			for (uint32_t lod = 0; lod < lodResource.capacity; lod++){
				// 配列サイズで確保
				lodResource.wvpResource = directXBase_->CreateBufferResource(sizeof(TransformationMatrix) * maxInstanceCount_);
				//座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
				//書き込むためのアドレス
				lodResource.wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&lodResource.wvpData));
				//単位行列を書き込んでおく
				for (uint32_t i = 0; i < static_cast<uint32_t>(maxInstanceCount_); i++){
					lodResource.wvpData[i].wvp = Matrix4x4::Identity4x4();
					lodResource.wvpData[i].world = Matrix4x4::Identity4x4();
					lodResource.wvpData[i].worldInverseTranspose = Matrix4x4::Identity4x4();
				}
			}
		}
	}
}

//座標変換行列リソースのストラクチャバッファの生成
void Object3dRenderer::CreateStructuredBufferForWvp(){
	for (Object3dGpuResource& objectResource : objectResources_){
		for (LODGpuResource& lodResource : objectResource.lodResources){
			for (uint32_t lod = 0; lod < lodResource.capacity; lod++){
				//ストラクチャバッファを生成
				lodResource.srvIndex = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
				srvManager_->CreateSRVForStructuredBuffer(
					lodResource.srvIndex,
					lodResource.wvpResource.Get(),
					static_cast<uint32_t>(maxInstanceCount_),
					sizeof(TransformationMatrix)
				);
			}
		}
	}
}
