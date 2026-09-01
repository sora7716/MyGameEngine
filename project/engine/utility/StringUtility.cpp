#include "StringUtility.h"
#include <Windows.h>

// stringをwstringに変換
std::wstring stringUtility::ConvertString(const std::string& str){
	if (str.empty()){
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0){
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

// wstringをstringに変換
std::string stringUtility::ConvertString(const std::wstring& str){
	if (str.empty()){
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0){
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

//文字列の末尾につく文字を削除
std::string stringUtility::RemoveSuffix(const std::string& base, std::string_view remove){
	std::string newStr = base;

	//省いた文字列
	const std::size_t removePos = base.find(remove);

	//最後がstring_viewで取得した文字列が含まれているか
	if (removePos != std::string_view::npos){
		newStr.erase(removePos);
	}

	return newStr;
}
