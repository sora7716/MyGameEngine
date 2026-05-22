#pragma once
#include <string>

/// <summary>
/// ログ
/// </summary>
class Log final{
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	static Log* GetInstance();

	/// <summary>
	/// コンソールプリント(ロガー)
	/// </summary>
	/// <param name="message">メッセージ</param>
	static void ConsolePrintf(const std::string& message);

private://メンバ関数
	Log() = default;
	~Log() = default;
	Log(const Log&) = delete;
	const Log operator=(const Log&) = delete;
};

