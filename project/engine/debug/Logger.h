#pragma once
#include <string>

/// <summary>
/// ログ
/// </summary>
class Logger final{
public://メンバ関数
	/// <summary>
	/// コンソールプリント(ロガー)
	/// </summary>
	/// <param name="message">メッセージ</param>
	static void ConsolePrintf(const std::string& message);

	/// <summary>
	/// コンソールプリント(ロガー)
	/// </summary>
	/// <param name="message">メッセージ</param>
	static void ConsolePrintf(const std::wstring& message);
private://メンバ関数
	Logger() = default;
	~Logger() = default;
	Logger(const Logger&) = delete;
	const Logger operator=(const Logger&) = delete;
};

