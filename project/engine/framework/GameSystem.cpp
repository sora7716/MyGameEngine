#include "GameSystem.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"
//初期化
void GameSystem::Initialize() {
	Framework::Initialize();
	//タイトルシーンを呼び出す
	//core_->GetSceneManager()->ChangeScene("Title");
	core_->GetSceneManager()->ChangeScene("TestPlay");
#ifdef _DEBUG
	//シーンの管理
	core_->GetSceneManager()->Update();
	//デバッグしたいシーンを呼び出す
	core_->GetSceneManager()->ChangeScene("TestPlay");
#endif // _DEBUG
}

//更新
void GameSystem::Update() {
	Framework::Update();
	//ウィンドウの検索キーがウィンドウの数を超えてしまった場合
	if (windowIndex_ >= static_cast<uint32_t>(WindowType::kWindowTypeCount)) {
		windowIndex_ = 0;
	}
}

//デバッグ
void GameSystem::Debug() {
	Framework::Debug();
}

//描画
void GameSystem::Draw() {
#ifdef _DEBUG
#else
	windowIndex_ = 0;
#endif // _DEBUG
	//描画開始位置
	core_->GetDirectXBase()->PreDraw(windowIndex_);
	//SRVの管理
	core_->GetSRVManager()->PreDraw();
	//シーン
	core_->GetSceneManager()->Draw();

	//デバッグ画面のときにしか表示しない
	if (core_->GetWinApi()->GetHwnd(windowIndex_) == core_->GetWinApi()->GetHwnd(WindowType::kDebug)) {
		//ImGuiの管理
		core_->GetImGuiManager()->Draw();
	}

	//描画終了位置
	core_->GetDirectXBase()->PostDraw(windowIndex_);

	//ウィンドウの検索キーを加算
	windowIndex_++;
}

//終了
void GameSystem::Finalize() {
	Framework::Finalize();
}
