#pragma once
#include <string>
#include <fstream>

// <summary>
/// ログ
/// </summary>
class Logger{
public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

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

	/// <summary>
	/// ログの書き出し
	/// </summary>
	/// <param name="message">メッセージ</param>
	static void OutputLog(const std::string& message);

	/// <summary>
	/// ログの書き出し
	/// </summary>
	/// <param name="message">メッセージ</param>
	static void OutputLog(const std::wstring& message);
private:
	//ログのストリーム
	static inline std::ofstream logStream;
};

