#include "Framework.h"
#include "MathUtility.h"
#include "WinApi.h"
#include "Input.h"
#include "SceneManager.h"
#include "LightingManager.h"
#include "Logger.h"
#include "Input.h"
#include "DirectXBase.h"
#include "RTVManager.h"
#include "DSVManager.h"

//コンストラクタ
Framework::Framework(){

}

//デストラクタ
Framework::~Framework(){
	//RTVの解放
	for (uint32_t rtvIndex : rtvIndices_){
		core_->GetRTVManager()->Free(rtvIndex);
	}
	//DSVの解放
	core_->GetDSVManager()->Free(dsvIndex_);
}

//初期化
void Framework::Initialize(){
	//ログの初期化
	Logger::Initialize();
	//エンジンの核
	core_ = std::make_unique<Core>();
	core_->Initialize();

	//DirectXの基盤部分
	DirectXBase* directXBase = core_->GetDirectXBase();
	//RTVの管理
	RTVManager* rtvManager = core_->GetRTVManager();
	//DSVの管理
	DSVManager* dsvManager = core_->GetDSVManager();

	//RTVの生成
	const uint32_t swapChainCount = directXBase->GetSwapChainResourceSize();
	//サイズを設定
	rtvIndices_.resize(swapChainCount);

	for (uint32_t i = 0; i < swapChainCount; i++){
		const uint32_t rtvIndex = rtvManager->Allocate();
		rtvManager->CreateRTV(directXBase->GetSwapChainResources()[i].Get(), rtvIndex, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		//検索キーを保存
		rtvIndices_[i] = rtvIndex;
	}

	//DSVの生成
	dsvIndex_ = dsvManager->Allocate();
	dsvManager->CreateDSV(directXBase->GetDepthStencilTexture(), dsvIndex_, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

//更新
void Framework::Update(){
	//入力処理
	core_->GetInput()->Update();
	//ライトの管理
	core_->GetLightingManager()->Update();
	//シーンの管理
	core_->GetSceneManager()->Update();
}

//デバッグ
void Framework::Debug(){
	//シーンの管理
	core_->GetSceneManager()->Debug();
}

//終了
void Framework::Finalize(){
}

//ゲームループ
void Framework::Run(){
	//ゲームシステムの初期化
	Initialize();
	//ウィンドウの✖ボタンが押されるまでループ
	while (isEndRequest()){
		//ゲームシステムの更新
		Update();

		//デバッグ
		Debug();

		//ゲームシステムの描画
		Draw();
#ifdef _DEBUG
		//エスケイプを押したらループを抜ける
		if (core_->GetInput()->TriggerKey(DIK_ESCAPE) && core_->GetInput()->PressKey(DIK_LSHIFT)){
			break;
		}
#endif // _DEBUG
	}
	//ゲームシステムの終了
	Finalize();
}

//終了リクエスト
bool Framework::isEndRequest(){
	return core_->GetWinApi()->ProcessMessage();
}
