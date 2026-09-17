#include "Input.h"
#include "WinApi.h"
#include "Camera.h"
#include "MatrixUtility.h"
#include "MathUtility.h"
#include <cassert>
#include <climits>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

//生成
std::unique_ptr<Input> Input::Create(ConstructorKey key, WinApi* winApi){
	//生成
	std::unique_ptr<Input>instance = std::make_unique<Input>(key);
	//初期化
	instance->Initialize(winApi);

	return instance;
}

//コンストラクタ
Input::Input(ConstructorKey){
}

//デストラクタ
Input::~Input(){
}

//初期化
void Input::Initialize(WinApi* winApi){
	//ウィンドウズアプリケーションを受け取る
	winApi_ = winApi;
	//キーボード入力
	KeyboardInitialize();
	//マウス入力
	MouseInitialize();
	//デッドゾーンを設定
	for (uint32_t i = 0; i < 4; i++){
		xboxPadDatas_[i].deadZone = 0.1f;
	}
}

//更新
void Input::Update(){
	//キーボード入力
	KeyboardUpdate();
	//マウス入力
	MouseUpdate();
	//XboxPad
	XboxPadUpdate();
}

// キーの押下をチェック
bool Input::PressKey(BYTE keyNumber){
	//指定キーを押していればtrueを返す
	if (keys_[keyNumber]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

// キーを押下した瞬間をチェック
bool Input::TriggerKey(BYTE keyNumber){
	//キーを押下した瞬間ならばtrueを返す
	if (keys_[keyNumber] && !preKeys_[keyNumber]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

// キーを離した瞬間をチェック
bool Input::ReleaseTriggerKey(BYTE keyNumber){
	//キーを離した瞬間ならばtrueを返す
	if (!keys_[keyNumber] && preKeys_[keyNumber]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//キーを離した事をチェック
bool Input::ReleaseKey(BYTE keyNumber){
	//キーを離した瞬間ならばtrueを返す
	if (!keys_[keyNumber] && !preKeys_[keyNumber]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//マウスのボタンの押下をチェック
bool Input::PressMouseButton(Click mouseClickPos){
	//マウスの押していればtrueを返す
	if (mouseState_.rgbButtons[static_cast<uint32_t>(mouseClickPos)]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//マウスのボタンの押下した瞬間をチェック
bool Input::TriggerMouseButton(Click mouseClickPos){
	//マウスの押していればtrueを返す
	if (mouseState_.rgbButtons[static_cast<uint32_t>(mouseClickPos)] && !preMouseState_.rgbButtons[static_cast<uint32_t>(mouseClickPos)]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//マウスのボタンを話した瞬間をチェック
bool Input::ReleaseTriggerMouseButton(Click mouseClickPos){
	//マウスの押していればtrueを返す
	if (!mouseState_.rgbButtons[static_cast<uint32_t>(mouseClickPos)] && preMouseState_.rgbButtons[static_cast<uint32_t>(mouseClickPos)]){
		return true;
	}
	//そうでなければfalseを返す
	return false;
}

//マウスの移動量のゲッター
Vector2 Input::GetMouseMoveAmount() const{
	Vector2 result = { static_cast<float>(mouseState_.lX),static_cast<float>(mouseState_.lY) };
	return result;
}

//マウスホイールの回転量のゲッター
float Input::GetWheelRotate() const{
	float result = static_cast<float>(mouseState_.lZ);
	return result;
}

//ワールド座標系のマウスの位置のゲッター
Vector3 Input::GetWorldMousePosition(Camera* camera) const{
	// マウスの座標
	POINT mousePosition;
	// マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = winApi_->GetActiveHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// ビューポートサイズを取得
	float windowWidth = static_cast<float>(WinApi::kClientWidth);
	float windowHeight = static_cast<float>(WinApi::kClientHeight);

	// マウス座標を正規化デバイス座標系(NDC)に変換
	float mouseX = (2.0f * mousePosition.x) / windowWidth - 1.0f;
	float mouseY = 1.0f - (2.0f * mousePosition.y) / windowHeight;
	Vector3 posNDC = { mouseX, mouseY, 1.0f }; // 遠平面(z = 1.0f)でのNDC

	// ビュープロジェクション行列の逆行列を取得
	Matrix4x4 matInverseVP = (camera->GetViewMatrix() * camera->GetProjectionMatrix()).Inverse();

	// NDCからワールド座標系への変換（遠平面）
	Vector3 posFar = posNDC * matInverseVP;

	// NDCを近平面(z = 0)に調整
	posNDC.z = 0.0f;
	Vector3 posNear = posNDC * matInverseVP;

	// マウスレイの方向を計算
	Vector3 mouseDirection = (posFar - posNear).Normalize();
	return mouseDirection;
}

//スクリーン座標系のマウスの位置のゲッター
Vector2 Input::GetMousePosition() const{
	// マウスの座標
	POINT mousePosition;
	// マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = winApi_->GetActiveHwnd();
	ScreenToClient(hwnd, &mousePosition);
	Vector2 result = { static_cast<float>(mousePosition.x),static_cast<float>(mousePosition.y) };
	return result;
}

//Xboxが接続できたかどうか
bool Input::IsXboxPadConnected(DWORD xBoxPadNumber){
	return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].isConnected;
}

//XboxPadのボタンの押下をチェック
bool Input::PressXboxPad(DWORD xBoxPadNumber, XboxInput xboxButton){
	if (xboxButton == XboxInput::kLT){
		//LT
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.leftTriggerButton = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftTrigger > 0.0f;
		return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.leftTriggerButton;
	} else if (xboxButton == XboxInput::kRT){
		//RT
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.rightTriggerButton = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightTrigger > 0.0f;
		return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.rightTriggerButton;
	}
	//それ以外
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.button = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].state.Gamepad.wButtons & static_cast<WORD>(xboxButton);
	return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.button;
}

//XboxPadのボタンの押下した瞬間をチェック
bool Input::TriggerXboxPad(DWORD xBoxPadNumber, XboxInput xboxButton){
	if (xboxButton == XboxInput::kLT){
		//LT
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.leftTriggerButton = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftTrigger > 0.0f;
		return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.leftTriggerButton && !xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].preButton.leftTriggerButton;
	} else if (xboxButton == XboxInput::kRT){
		//RT
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.rightTriggerButton = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightTrigger > 0.0f;
		return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.rightTriggerButton && !xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].preButton.rightTriggerButton;
	}
	//それ以外
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.button = PressXboxPad(static_cast<uint32_t>(xBoxPadNumber), xboxButton);
	return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.button && !xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].preButton.button;
}

//XboxPadのボタンの離した瞬間をチェック
bool Input::ReleaseTriggerXboxPad(DWORD xBoxPadNumber, XboxInput xboxButton){
	if (xboxButton == XboxInput::kLT){
		//LT
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.leftTriggerButton = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftTrigger > 0.0f;
		return !xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.leftTriggerButton && xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].preButton.leftTriggerButton;
	} else if (xboxButton == XboxInput::kRT){
		//RT
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.rightTriggerButton = xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightTrigger > 0.0f;
		return !xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.rightTriggerButton && xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].preButton.rightTriggerButton;
	}
	//それ以外
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.button = PressXboxPad(static_cast<uint32_t>(xBoxPadNumber), xboxButton);
	return !xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].currButton.button && xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].preButton.button;
}

//Xboxの左スティックのゲッター
const Vector2 Input::GetXboxPadLeftStick(DWORD xBoxPadNumber){
	//横軸
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick.x = static_cast<float>(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].state.Gamepad.sThumbLX) / static_cast<float>(SHRT_MAX);

	//縦軸
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick.y = static_cast<float>(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].state.Gamepad.sThumbLY) / static_cast<float>(SHRT_MAX);

	//横軸の場合のデッドゾーン
	if (std::fabs(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick.x) < xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].deadZone){
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick.x = 0.0f;
	}

	//縦軸の場合のデッドゾーン
	if (std::fabs(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick.y) < xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].deadZone){
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick.y = 0.0f;
	}

	return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].leftStick;
}

//Xboxの右スティックのゲッター
const Vector2 Input::GetXboxPadRightStick(DWORD xBoxPadNumber){
	//横軸
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick.x = static_cast<float>(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].state.Gamepad.sThumbRX) / static_cast<float>(SHRT_MAX);

	//縦軸
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick.y = static_cast<float>(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].state.Gamepad.sThumbRY) / static_cast<float>(SHRT_MAX);

	//横軸の場合のデッドゾーン
	if (std::fabs(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick.x) < xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].deadZone){
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick.x = 0.0f;
	}

	//縦軸の場合のデッドゾーン
	if (std::fabs(xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick.y) < xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].deadZone){
		xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick.y = 0.0f;
	}

	return xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].rightStick;
}

//Xboxのデッドゾーン
void Input::SetDeadZone(DWORD xBoxPadNumber, float deadZone){
	xboxPadDatas_[static_cast<uint32_t>(xBoxPadNumber)].deadZone = deadZone;
}

//キーボード入力の初期化
void Input::KeyboardInitialize(){
	HRESULT result = S_FALSE;
	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(winApi_->GetWndClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(winApi_->GetHwnd(WindowType::kGame), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

//キーボード入力の更新
void Input::KeyboardUpdate(){
	HRESULT result = S_FALSE;
	// 現在の状態を前回の状態として保存
	memcpy(preKeys_, keys_, sizeof(keys_));
	//キーボード情報の取得開始
	result = keyboard_->Acquire();
	result = keyboard_->GetDeviceState(sizeof(keys_), keys_);
}

//マウス入力の初期化
void Input::MouseInitialize(){
	HRESULT result = S_FALSE;
	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(winApi_->GetWndClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = mouse_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = mouse_->SetCooperativeLevel(winApi_->GetHwnd(WindowType::kGame), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

//マウス入力の更新
void Input::MouseUpdate(){
	HRESULT result = S_FALSE;
	// 現在の状態を前回の状態として保存
	memcpy(&preMouseState_, &mouseState_, sizeof(DIMOUSESTATE));
	//キーボード情報の取得開始
	result = mouse_->Acquire();
	result = mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);
}

//XboxPadの更新
void Input::XboxPadUpdate(){
	//コントローラを接続したかどうか
	for (uint32_t i = 0; i < 4; i++){
		DWORD result = XInputGetState(0, &xboxPadDatas_[i].state);
		xboxPadDatas_[i].isConnected = result == ERROR_SUCCESS;
	}

	//接続されてなければ
	if (!xboxPadDatas_[0].isConnected){
		return;//早期リターン
	}

	//過去のボタンを取得
	xboxPadDatas_[0].preButton.button = xboxPadDatas_[0].currButton.button;
	xboxPadDatas_[0].preButton.leftTriggerButton = xboxPadDatas_[0].currButton.leftTriggerButton;
	xboxPadDatas_[0].preButton.rightTriggerButton = xboxPadDatas_[0].currButton.rightTriggerButton;

	//LTとRTの押し込んだかどうか計測
	xboxPadDatas_[0].leftTrigger = static_cast<float>(xboxPadDatas_[0].state.Gamepad.bLeftTrigger) / static_cast<float>(UCHAR_MAX);
	xboxPadDatas_[0].rightTrigger = static_cast<float>(xboxPadDatas_[0].state.Gamepad.bRightTrigger) / static_cast<float>(UCHAR_MAX);
}
