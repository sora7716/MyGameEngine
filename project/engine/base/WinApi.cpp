#include "WinApi.h"
#pragma comment(lib,"winmm.lib")
#include "Vector2.h"
#include <cassert>
#include "resources/resource.h"
#ifdef USE_IMGUI
#include "imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // USE_IMGUI

//生成
std::unique_ptr<WinApi> WinApi::Create(ConstructorKey key){
	//生成
	std::unique_ptr<WinApi>instance = std::make_unique<WinApi>(key);
	//初期化
	instance->Initialize();

	return instance;
}

//ウィンドウプロシージャ
LRESULT WinApi::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	//WinApi
	WinApi* winApi = reinterpret_cast<WinApi*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (msg == WM_NCCREATE){
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		winApi = static_cast<WinApi*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(winApi));
	}

	//マウスカーソルの表示非表示
	if (msg == WM_SETCURSOR){
		if (winApi){
			if (LOWORD(lParam) == HTCLIENT){
				////ゲーム画面ではカーソルを非表示
				//if (hwnd == winApi->GetHwnd(WindowType::kGame)){
				//	SetCursor(nullptr);
				//	return TRUE;
				//}

				SetCursor(LoadCursor(nullptr, IDC_ARROW));
				return TRUE;
			}
		}
	}

#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)){
		return true;
	}
#endif // USE_IMGUI

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg){
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

//コンストラクタ
WinApi::WinApi(ConstructorKey){}

//デストラクタ
WinApi::~WinApi(){
	CloseWindow(hwnd_);
	CoUninitialize();
}

// ウィンドウの生成するための初期化
void WinApi::Initialize(){
	HRESULT hr = S_FALSE;
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);
	//メインスレッドではMTAでCOMを利用
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	//WNDCLASSEXのサイズを設定
	wndClass_.cbSize = sizeof(WNDCLASSEX);
	//ウィンドウプロシージャ
	wndClass_.lpfnWndProc = WindowProc;
	//ウィンドウのクラス名
	wndClass_.lpszClassName = L"WindowClass";
	//インスタンスハンドル
	wndClass_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wndClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//タスクバーのアイコンを設定
	wndClass_.hIcon = LoadIcon(wndClass_.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	//タイトルバーのアイコンを設定
	wndClass_.hIconSm = LoadIcon(wndClass_.hInstance, MAKEINTRESOURCE(IDI_ICON2));

	//ウィンドウクラスを登録する
	RegisterClassEx(&wndClass_);

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	windowRect_ = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズをwrcを変更してもらう
	AdjustWindowRect(&windowRect_, WS_OVERLAPPEDWINDOW, false);

	Vector2Int windowPos = { CW_USEDEFAULT,CW_USEDEFAULT };

	//ウィンドウの作成
		//ウィンドウを作成
	hwnd_ = CreateWindow(
	wndClass_.lpszClassName,//利用するクラス
	label_.c_str(),
	WS_OVERLAPPEDWINDOW,//よく見るウィンドウのスタイル
	windowPos.x,//ウィンドウの表示位置(X座標)
	windowPos.y,//ウィンドウの表示位置(Y座標)
	windowRect_.right - windowRect_.left,//ウィンドウの横幅
	windowRect_.bottom - windowRect_.top,//ウィンドウの縦幅
	nullptr,
	nullptr,
	wndClass_.hInstance,//インスタンスハンドル
	this
	);

	//ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

// プロセスメッセージ
bool WinApi::ProcessMessage(){
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
HWND WinApi::GetHwnd() const{
	return  hwnd_;
}

//WNDクラスのゲッター
WNDCLASSEX WinApi::GetWndClass()const{
	return wndClass_;
}

//マウスカーソルの表示非表示
void WinApi::SetShowCursor(bool isShowCursor){
	if (isShowCursor_ != isShowCursor){
		isShowCursor_ = isShowCursor;
		ShowCursor(isShowCursor);
	}
}