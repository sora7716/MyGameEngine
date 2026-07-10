#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <array>

//前方宣言
class Core;

//ウィンドウタイプ
enum class WindowType :uint32_t {
	kGame,
	kDebug,
	kWindowTypeCount
};

/// <summary>
/// ウィンドウズAPI
/// </summary>
class WinApi {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~WinApi();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// プロセスメッセージ
	/// </summary>
	/// <returns>プロセスメッセージ</returns>
	bool ProcessMessage();

	/// <summary>
	/// HWNDの取得
	/// </summary>
	/// <param name="windowIndex">ウィンドウの検索キー</param>
	/// <returns>HWND</returns>
	HWND GetHwnd(uint32_t windowIndex)const;

	/// <summary>
	/// HWNDの取得
	/// </summary>
	/// <param name="windowType">ウィンドウタイプ</param>
	/// <returns>HWND</returns>
	HWND GetHwnd(WindowType windowType)const;

	/// <summary>
	/// 現在使用しているウィンドウのハンドルを取得
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	HWND GetActiveHwnd()const;

	/// <summary>
	/// WNDクラスのゲッター
	/// </summary>
	/// <returns>wndClass</returns>
	WNDCLASS GetWndClass()const;

	//コピーコンストラクタ禁止
	WinApi(const WinApi&) = delete;
	//代入演算子禁止
	const WinApi& operator=(const WinApi&) = delete;
public://PassKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit WinApi(ConstructorKey);
public://静的メンバ関数
	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">メッセージが送信されたウィンドウのハンドル</param>
	/// <param name="msg">メッセージの識別子</param>
	/// <param name="wParam">メッセージの追加情報</param>
	/// <param name="lParam">メッセージの追加情報</param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public://定数
	//画面の横幅
	static inline const int32_t kClientWidth = 1280;
	//画面の縦幅
	static inline const int32_t kClientHeight = 720;
	//ウィンドウの数
	static inline const uint32_t kWindowCount = static_cast<uint32_t>(WindowType::kWindowTypeCount);
	//タイトル名
	static inline const std::array<std::wstring, kWindowCount> labels_ = {
		L"Game",
		L"Debug",
	};
private://メンバ変数
	WNDCLASS wndClass_{};	//ウィンドウクラス
	std::array<HWND, kWindowCount> hwnds_;	//ウィンドウハンドル
	//今アクティブなウィンドウハンドル
	static inline HWND activeHwnd_ = nullptr;
	RECT windowRect_{};
};

