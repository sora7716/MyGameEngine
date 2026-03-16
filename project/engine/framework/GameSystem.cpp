#include "GameSystem.h"
#include "engine/scene/SceneManager.h"
#include "engine/scene/SceneFactory.h"
//初期化
void GameSystem::Initialize() {
	Framework::Initialize();
	//タイトルシーンを呼び出す
	core_->GetSceneManager()->ChangeScene("Title");
#ifdef _DEBUG
	//シーンの管理
	core_->GetSceneManager()->Update();
	//デバッグしたいシーンを呼び出す
	core_->GetSceneManager()->ChangeScene("Game");
#endif // _DEBUG
}

//更新
void GameSystem::Update() {
	Framework::Update();
}

//描画
void GameSystem::Draw() {
	//描画開始位置
	core_->GetDirectXBase()->PreDraw();
	//SRVの管理
	core_->GetSRVManager()->PreDraw();
	//シーン
	core_->GetSceneManager()->Draw();
	//ImGuiの管理
	core_->GetImGuiManager()->Draw();
	//描画終了位置
	core_->GetDirectXBase()->PostDraw();
}

//終了
void GameSystem::Finalize() {
	Framework::Finalize();
}
