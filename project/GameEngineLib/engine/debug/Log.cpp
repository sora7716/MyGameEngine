#include "Log.h"
#include <Windows.h>

//インスタンスのゲッター
Log* Log::GetInstance(){
    static Log instance;
    return &instance;
}
//コンソールプリント
void Log::ConsolePrintf(const std::string& message){
	OutputDebugStringA(message.c_str());
}
