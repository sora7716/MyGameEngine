#pragma once
#include <string>
#include <format>

/// <summary>
/// 文字を変換
/// </summary>
class StringUtility {
public://メンバ関数
	/// <summary>
    /// stringをwstringに変換
    /// </summary>
    /// <param name="str">string</param>
    /// <returns>wstring</returns>
	static std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstringをstringに変換
	/// </summary>
	/// <param name="str">wstirng</param>
	/// <returns>string</returns>
	static std::string ConvertString(const std::wstring& str);
};

