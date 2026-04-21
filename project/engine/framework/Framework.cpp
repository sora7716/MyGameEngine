#include "Framework.h"
#include "algorithm/Math.h"
#include "Input.h"

//初期化
void Framework::Initialize() {
	//エンジンの核
	core_ = std::make_unique<Core>();
	core_->Initialize();
	//カメラの設定
	core_->GetObject2dCommon()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	core_->GetObject3dCommon()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	core_->GetWireframeObject3dCommon()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
	core_->GetParticleCommon()->SetDefaultCamera(core_->GetCameraManager()->FindCamera("defaultCamera"));
}

//更新
void Framework::Update() {
	//入力処理
	core_->GetInput()->Update();
	//カメラの管理
	core_->GetCameraManager()->Update();
	//シーンの管理
	core_->GetSceneManager()->Update();
}

//終了
void Framework::Finalize() {
}

//ゲームループ
void Framework::Run() {
	//ゲームシステムの初期化
	Initialize();
	//ウィンドウの✖ボタンが押されるまでループ
	while (isEndRequest()) {
		//ゲームシステムの更新
		Update();
		//ゲームシステムの描画
		Draw();
#ifdef _DEBUG
		//エスケイプを押したらループを抜ける
		if (core_->GetInput()->TriggerKey(DIK_ESCAPE) && core_->GetInput()->PressKey(DIK_LSHIFT)) {
			break;
		}
#endif // _DEBUG
	}
	//ゲームシステムの終了
	Finalize();
}

//終了リクエスト
bool Framework::isEndRequest() {
	return core_->GetWinApi()->ProcessMessage();
}
