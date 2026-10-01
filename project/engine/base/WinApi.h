#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <array>
#include <memory>

//前方宣言
class Core;

/// <summary>
/// ウィンドウズAPI
/// </summary>
class WinApi{
public://PassKey
	class ConstructorKey{
	private:
		ConstructorKey() = default;
		friend class Core;
	};
public://静的メンバ関数
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="key">コンストラクタのKey</param>
	/// <returns>インスタンス</returns>
	static std::unique_ptr<WinApi>Create(ConstructorKey key);

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">メッセージが送信されたウィンドウのハンドル</param>
	/// <param name="msg">メッセージの識別子</param>
	/// <param name="wParam">メッセージの追加情報</param>
	/// <param name="lParam">メッセージの追加情報</param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PassKeyを受け取る</param>
	explicit WinApi(ConstructorKey);

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
	/// <returns>HWND</returns>
	HWND GetHwnd()const;

	/// <summary>
	/// WNDクラスのゲッター
	/// </summary>
	/// <returns>wndClass</returns>
	WNDCLASS GetWndClass()const;

	/// <summary>
	/// マウスカーソルの表示非表示の設定
	/// </summary>
	/// <param name="isShowCursor">マウスカーソルを表示非表示</param>
	void SetShowCursor(bool isShowCursor);
private://メンバ関数
	//コピーコンストラクタ禁止
	WinApi(const WinApi&) = delete;
	//代入演算子禁止
	const WinApi& operator=(const WinApi&) = delete;
public://定数
	//画面の横幅
	static inline const int32_t kClientWidth = 1280;
	//画面の縦幅
	static inline const int32_t kClientHeight = 720;

	//タイトル名
	static inline const std::wstring label_ = { L"Game" };
private://メンバ変数
	WNDCLASS wndClass_ = {};	//ウィンドウクラス
	HWND hwnd_ = {};	//ウィンドウハンドル
	//今アクティブなウィンドウハンドル
	static inline HWND activeHwnd_ = nullptr;
	RECT windowRect_{};
	//マウスカーソルの表示非表示
	bool isShowCursor_ = true;
};

