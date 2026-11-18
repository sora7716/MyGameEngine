#pragma once
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#include <cstdint>

//マウスのクリック位置
enum class ClickType {
	kLeft,
	kRight,
	kMiddle
};

//Xboxのボタンの位置
enum class XboxInputType :WORD {
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

//ボタンの状態
enum class InputPhase :uint32_t {
	kPressed = 1 << 0,//押した瞬間「
	kHeld = 1 << 1,//押しっぱなし
	kReleased = 1 << 2,//離した瞬間
	kIdle = 1 << 3//離しっぱなし
};

//スティックの種類
enum class StickType :uint32_t {
	kLeft,
	kRight
};