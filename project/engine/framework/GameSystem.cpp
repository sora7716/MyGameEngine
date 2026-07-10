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
}

//描画
void GameSystem::Draw() {
#ifdef _DEBUG
	uint32_t sceneCount = static_cast<uint32_t>(WindowType::kWindowTypeCount);
#else
	uint32_t sceneCount = 1;
#endif // _DEBUG


	for (uint32_t i = 0; i < sceneCount; i++) {
		//描画開始位置
		core_->GetDirectXBase()->PreDraw(i);
		//SRVの管理
		core_->GetSRVManager()->PreDraw();
		//シーン
		core_->GetSceneManager()->Draw();
		//デバッグ画面のときにしか表示しない
		if (core_->GetWinApi()->GetHwnd(i) == core_->GetWinApi()->GetHwnd(WindowType::kDebug)) {
			//ImGuiの管理
			core_->GetImGuiManager()->Draw();
		}
		//描画終了位置
		core_->GetDirectXBase()->PostDraw(i);
	}
}

//終了
void GameSystem::Finalize() {
	Framework::Finalize();
}
