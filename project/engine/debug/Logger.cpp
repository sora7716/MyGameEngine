#include "Logger.h"
#include <Windows.h>
#include "StringUtility.h"

//コンソールプリント
void logger::ConsolePrintf(const std::string& message){
	OutputDebugStringA(message.c_str());

}

//コンソールプリント
void logger::ConsolePrintf(const std::wstring& message){
	OutputDebugStringA(stringUtility::ConvertString(message).c_str());

}
