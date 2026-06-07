#include "WinApi.h"
#pragma comment(lib,"winmm.lib")
#include "imgui/imgui_impl_win32.h"
#include "Vector2.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//デストラクタ
WinApi::~WinApi() {
	for (HWND& hwnd : hwnds_) {
		CloseWindow(hwnd);
	}
	CoUninitialize();
}

// ウィンドウの生成するための初期化
void WinApi::Initialize() {
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);
	//メインスレッドではMTAでCOMを利用
	HRESULT result = CoInitializeEx(0, COINIT_MULTITHREADED);
	//ウィンドウプロシージャ
	wndClass_.lpfnWndProc = WindowProc;
	//ウィンドウのクラス名
	wndClass_.lpszClassName = L"CG2WindowClass";
	//インスタンスハンドル
	wndClass_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録する
	RegisterClass(&wndClass_);

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	windowRect_ = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズをwrcを変更してもらう
	AdjustWindowRect(&windowRect_, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウハンドルの数を設定
	hwnds_.resize(2);

	Vector2Int windowPos = {
		.x = 500,
		.y = -1000
	};

	hwnds_[0] = CreateWindow(
		wndClass_.lpszClassName,//利用するクラス
		label.c_str(),
		WS_OVERLAPPEDWINDOW,//よく見るウィンドウのスタイル
		CW_USEDEFAULT,//ウィンドウの表示位置(X座標)
		CW_USEDEFAULT,//ウィンドウの表示位置(Y座標)
		windowRect_.right - windowRect_.left,//ウィンドウの横幅
		windowRect_.bottom - windowRect_.top,//ウィンドウの縦幅
		nullptr,
		nullptr,
		wndClass_.hInstance,//インスタンスハンドル
		nullptr
	);

	hwnds_[1] = CreateWindow(
		wndClass_.lpszClassName,//利用するクラス
		L"DebugWindow",
		WS_OVERLAPPEDWINDOW,//よく見るウィンドウのスタイル
		CW_USEDEFAULT,//表示X座標(Windowに任せる)
		CW_USEDEFAULT,//表示Y座標(Windowに任せる)
		windowRect_.right - windowRect_.left,//ウィンドウの横幅
		windowRect_.bottom - windowRect_.top,//ウィンドウの縦幅
		nullptr,
		nullptr,
		wndClass_.hInstance,//インスタンスハンドル
		nullptr
	);
	debugHwnd_ = hwnds_[1];

	////ウィンドウを表示する
	//for (const HWND& hwnd : hwnds_) {
	//}
	ShowWindow(hwnds_[0], SW_SHOW);
}

// プロセスメッセージ
bool WinApi::ProcessMessage() {
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	if (msg.message == WM_QUIT) {
		return false;
	}
	return true;
}

//HWNDのゲッター
HWND WinApi::GetHwnd(uint32_t index)const {
	return hwnds_[index];
}

//WNDクラスのゲッター
WNDCLASS WinApi::GetWndClass()const {
	return wndClass_;
}

//コンストラクタ
WinApi::WinApi(ConstructorKey) {}

//ウィンドウプロシージャ
LRESULT WinApi::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//if (hwnd == debugHwnd_) {
	//	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
	//		return true;
	//	}
	//}
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
