#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <vector>

//前方宣言
class Core;

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
	/// HWNDのゲッター
	/// </summary>
	/// <param name="index">インデックス</param>
	/// <returns>HWND</returns>
	HWND GetHwnd(uint32_t index = 0)const;

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
	/// <param name="wparam">メッセージの追加情報</param>
	/// <param name="lparam">メッセージの追加情報</param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public://定数
	//画面の横幅
	static inline const int32_t kClientWidth = 1280;
	//画面の縦幅
	static inline const int32_t kClientHeight = 720;
	//タイトル名
	static inline const std::wstring label = L"CG3";
private://メンバ変数
	WNDCLASS wndClass_{};	//ウィンドウクラス
	std::vector<HWND> hwnds_ = { nullptr };	//ウィンドウハンドル
	static inline HWND debugHwnd_ = nullptr;
	RECT windowRect_{};
};

