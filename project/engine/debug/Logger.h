#pragma once
#include <string>

/// <summary>
/// ログ
/// </summary>
namespace logger{
	/// <summary>
	/// コンソールプリント(ロガー)
	/// </summary>
	/// <param name="message">メッセージ</param>
	void ConsolePrintf(const std::string& message);

	/// <summary>
	/// コンソールプリント(ロガー)
	/// </summary>
	/// <param name="message">メッセージ</param>
	void ConsolePrintf(const std::wstring& message);
};

