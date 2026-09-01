#pragma once
#include <string>
#include <format>

/// <summary>
/// 文字を変換
/// </summary>
namespace stringUtility {
	/// <summary>
	/// stringをwstringに変換
	/// </summary>
	/// <param name="str">string</param>
	/// <returns>wstring</returns>
	std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstringをstringに変換
	/// </summary>
	/// <param name="str">wstring</param>
	/// <returns>string</returns>
	std::string ConvertString(const std::wstring& str);

	/// <summary>
	/// 文字列の末尾につく文字を削除
	/// </summary>
	/// <param name="base">元の文字列</param>
	/// <param name="remove">消したい部分</param>
	/// <returns>消したい部分を排除した文字列</returns>
	std::string RemoveSuffix(const std::string& base, std::string_view remove);
};

