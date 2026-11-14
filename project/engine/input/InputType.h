#pragma once
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>

//マウスのクリック位置
enum class Click {
	kLeft,
	kRight,
	kMiddle
};

//Xboxのボタンの位置
enum class XboxInput :WORD {
	kDPadUp = XINPUT_GAMEPAD_DPAD_UP,
	kDPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,
	kDPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
	kDPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
	kY = XINPUT_GAMEPAD_Y,
	kB = XINPUT_GAMEPAD_B,
	kA = XINPUT_GAMEPAD_A,
	kX = XINPUT_GAMEPAD_X,
	kRB = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	kLB = XINPUT_GAMEPAD_LEFT_SHOULDER,
	kRTHUMB = XINPUT_GAMEPAD_RIGHT_THUMB,
	kLTHUMB = XINPUT_GAMEPAD_LEFT_THUMB,
	kStart = XINPUT_GAMEPAD_START,
	kBack = XINPUT_GAMEPAD_BACK,
	kRT,
	kLT,
};
