#include "framework/GameSystem.h"
#include "engine/base/D3DResourceLeakChecker.h"
#include "CrashHandler.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//誰も捕捉しなかった場合に(Unhandled)、補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	//メモリーリークをチェック
	D3DResourceLeakChecker leakChecker;

	uint32_t* p = nullptr;
	*p = 100;

	//ゲームシステムの生成
	std::unique_ptr<Framework> gameSystem = std::make_unique<GameSystem>();

	//ゲームループ
	gameSystem->Run();
	return 0;
}