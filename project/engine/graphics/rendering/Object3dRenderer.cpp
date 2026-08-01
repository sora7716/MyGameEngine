#include "Object3dRenderer.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "Camera.h"
#include "Model.h"
#include "Logger.h"
#include <cassert>

//生成
std::unique_ptr<Object3dRenderer> Object3dRenderer::Create(DirectXBase* directXBase, SRVManager* srvManager, uint32_t maxInstance){
	std::unique_ptr<Object3dRenderer>instance = std::make_unique<Object3dRenderer>();
	instance->Initialize(directXBase, srvManager, maxInstance);
	return instance;
}

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

		for (uint32_t lodIndex = 0; lodIndex < renderData.lodRenderData.models.size(); lodIndex++){
			//WVPのSRVIndex
			uint32_t wvpSrvIndex = renderData.lodRenderData.srvIndices[lodIndex];
			//描画カウント
			uint32_t drawCount = renderData.lodRenderData.drawCounts[lodIndex];

			//LODモデルが存在してなかったら
			if (!renderData.lodRenderData.models[lodIndex]){
				continue;
			}

			//LOD描画カウントが0だったら
			if (drawCount == 0){
				continue;
			}

			//LODごとのWVP SRVを設定
			directXBase_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(wvpSrvIndex));

			//ドローコール
			renderData.lodRenderData.models[lodIndex]->Draw(drawCount);
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

	//LODの描画データのサイズチェック
	if (!renderData.lodRenderData.IsLodCountValid()){
		Logger::OutputLog("LODの描画データのサイズが合いません");
		assert(false);
	}

	//レンダーデータの追加
	renderDatas_.push_back(renderData);
}
