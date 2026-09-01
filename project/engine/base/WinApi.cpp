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
	HRESULT hr = S_FALSE;
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);
	//メインスレッドではMTAでCOMを利用
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
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

#ifdef _DEBUG
	uint32_t windowCount = kWindowCount;
#else 
	uint32_t windowCount = 1;
#endif // _DEBUG
	//ウィンドウの作成
	for (uint32_t i = 0; i < windowCount; i++) {
		//ウィンドウを作成
		hwnds_[i] = CreateWindow(
		wndClass_.lpszClassName,//利用するクラス
		(labels_[i]).c_str(),
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

		//ウィンドウを表示する
		ShowWindow(hwnds_[i], SW_SHOW);
	}

}

// プロセスメッセージ
bool WinApi::ProcessMessage() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		if (msg.message == WM_QUIT){
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return true;
}

//HWNDの取得
HWND WinApi::GetHwnd(uint32_t windowIndex) const {
	return  hwnds_[windowIndex];
}

//HWNDの取得
HWND WinApi::GetHwnd(WindowType windowType)const {
	return hwnds_[static_cast<uint32_t>(windowType)];
}

//現在使用しているウィンドウのハンドルを取得
HWND WinApi::GetActiveHwnd() const {
	return activeHwnd_;
}

//指定したウィンドウと今選択しているウィンドウが一致しているか
bool WinApi::IsActiveHwnd(WindowType windowType) const {
	return GetActiveHwnd() == GetHwnd(windowType);
}

//WNDクラスのゲッター
WNDCLASS WinApi::GetWndClass()const {
	return wndClass_;
}

//コンストラクタ
WinApi::WinApi(ConstructorKey) {}

//ウィンドウプロシージャ
LRESULT WinApi::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	case WM_SETFOCUS:
		//現在選択しているウィンドウハンドルを取得
		activeHwnd_ = hwnd;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
