#include "GameSystem.h"
#include "WinApi.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "DSVManager.h"
#include "RTVManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "SceneFactory.h"
#include "RenderSystem.h"

//コンストラクタ
GameSystem::GameSystem(){
}

//デストラクタ
GameSystem::~GameSystem(){
}

//初期化
void GameSystem::Initialize(){
	Framework::Initialize();
	//シーンを呼び出す
	core_->GetSceneManager()->ChangeScene("Game");
}

//更新
void GameSystem::Update(){
	Framework::Update();
	//ウィンドウの検索キーがウィンドウの数を超えてしまった場合
	if (windowIndex_ >= static_cast<uint32_t>(WindowType::kWindowTypeCount)){
		windowIndex_ = 0;
	}
}

//デバッグ
void GameSystem::Debug(){
	Framework::Debug();
}

//描画
void GameSystem::Draw(){
#ifdef _DEBUG
#else
	windowIndex_ = 0;
#endif // _DEBUG
	//バックバッファの検索キーを取得
	const uint32_t backBufferIndex = core_->GetDirectXBase()->GetBackBufferIndex(windowIndex_);

	//RTVハンドルの取得
	const uint32_t rtvIndex = rtvIndices_[backBufferIndex];
	const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = core_->GetRTVManager()->GetCPUDescriptorHandle(rtvIndex);

	//DSVハンドルの取得
	const D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = core_->GetDSVManager()->GetCPUDescriptorHandle(dsvIndex_);
	
	//描画開始位置
	core_->GetDirectXBase()->PreDraw(windowIndex_, rtvHandle, dsvHandle);
	//SRVの管理
	core_->GetSRVManager()->PreDraw();

	//デバッグ画面かどうか
	const bool isDebugWindow = core_->GetWinApi()->GetHwnd(windowIndex_) == core_->GetWinApi()->GetHwnd(WindowType::kDebug);

	//デバッグ画面のときにしか表示しない
	if (isDebugWindow){
		//シーン
		core_->GetSceneManager()->DebugDraw();
	} else{
		//シーン
		core_->GetSceneManager()->GameDraw();
	}

	//描画
	core_->GetRenderSystem()->Draw();

	//ImGuiの描画はDebug画面限定
	if (isDebugWindow){
		//ImGuiの管理
		core_->GetImGuiManager()->Draw();
	}

	//描画終了位置
	core_->GetDirectXBase()->PostDraw(windowIndex_);

	//ウィンドウの検索キーを加算
	windowIndex_++;
}

//終了
void GameSystem::Finalize(){
	Framework::Finalize();
}
