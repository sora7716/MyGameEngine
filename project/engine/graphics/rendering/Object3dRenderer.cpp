#include "Object3dRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "Camera.h"
#include "Model.h"
#include "Logger.h"
#include <cassert>

//コンストラクタ
Object3dRenderer::Object3dRenderer(){
}

//デストラクタ
Object3dRenderer::~Object3dRenderer(){
}

//初期化
void Object3dRenderer::Initialize(DirectXBase* directXBase, SRVManager* srvManager, uint32_t maxInstance){
	//DirectXの基盤部分のNullチェック
	assert(directXBase);
	directXBase_ = directXBase;
	//SRVの管理のNullチェック
	assert(srvManager);
	srvManager_ = srvManager;

	//インスタンスの最大値の記録
	maxInstanceCount_ = maxInstance;

	//描画データの初期化
	renderDatas_.reserve(maxInstanceCount_);
	renderDatas_.clear();
}

//描画
void Object3dRenderer::Draw(){
	for (const RenderData& renderData : renderDatas_){

		//カメラ
		renderData.renderCamera->DrawSetting(4);

		//平光源CBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootConstantBufferView(3, renderData.directionalLightResource->GetGPUVirtualAddress());
		//点光源のStructuredBufferの場所を設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(5, srvManager_->GetGPUDescriptorHandle(renderData.pointLightSrvIndex));
		//スポットライトのStructuredBufferを設定
		directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(6, srvManager_->GetGPUDescriptorHandle(renderData.spotLightSrvIndex));

		for (uint32_t lodIndex = 0; lodIndex < renderData.models.size(); lodIndex++){
			//モデル
			Model* model = renderData.models[lodIndex];
			//WVPのSRVIndex
			uint32_t wvpSrvIndex = renderData.lodSrvIndices[lodIndex];
			//描画カウント
			uint32_t drawCount = renderData.lodClodDrawCounts[lodIndex];

			//LODモデルが存在してなかったら
			if (!model){
				continue;
			}

			//LOD描画カウントが0だったら
			if (drawCount == 0){
				continue;
			}

			//LODごとのWVP SRVを設定
			directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(wvpSrvIndex));

			//ドローコール
			model->Draw(drawCount);

		}
	}
}

//描画データの追加
void Object3dRenderer::AddRenderData(const RenderData& renderData){
	//インスタンスの最大値
	if (maxInstanceCount_ <= renderDatas_.size()){
		Logger::OutputLog("想定していたインスタンスの最大値を超えてい追加しています");
		assert(false);
	}
	//レンダーデータの追加
	renderDatas_.push_back(renderData);
}
