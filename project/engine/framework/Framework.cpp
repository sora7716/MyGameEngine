#include "Framework.h"
#include "MathUtility.h"
#include "Logger.h"
#include "Input.h"

//初期化
void Framework::Initialize() {
	//ログの初期化
	Logger::Initialize();
	//エンジンの核
	core_ = std::make_unique<Core>();
	core_->Initialize();
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

//デバッグ
void Framework::Debug() {
	//シーンの管理
	core_->GetSceneManager()->Debug();
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

		if (core_->GetWinApi()->IsActiveHwnd(WindowType::kDebug)) {
			//デバッグのウィンドウの時だけ
			Debug();
		}

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
