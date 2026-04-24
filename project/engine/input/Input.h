#pragma once
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <memory>
#include <wrl.h>
#include "engine/math/Vector2.h"
#include "engine/math/Vector3.h"
#include "Xinput.h"
#include <cstdint>
//前方宣言
class WinApi;
class Core;
class Camera;

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

//Xboxのボタン用構造体
struct XboxButton {
	WORD button;
	bool leftTriggerButton;
	bool rightTriggerButton;
};

//XboxPadのデータ
struct XboxPadData {
	XINPUT_STATE state;//ステータス
	bool isConnected;//接続できたか
	XboxButton currButton;//現在のボタン
	XboxButton preButton;//過去のボタン
	float leftTrigger;//LTボタン
	float rightTrigger;//RTボタン
	Vector2 leftStick;//左スティック
	Vector2 rightStick;//右スティック
	float deadZone;//デッドゾーン
};

/// <summary>
/// 入力
/// </summary>
class Input final {
public://エイリアステンプレート
	//namespace省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Input();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">ウィンドウズアプリケーション</param>
	void Initialize(WinApi* winApi);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>押されてるか</returns>
	bool PressKey(BYTE keyNumber);

	/// <summary>
	/// キーを押下した瞬間をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>押した瞬間</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーを離した瞬間をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>離した瞬間</returns>
	bool ReleaseTriggerKey(BYTE keyNumber);

	/// <summary>
	/// マウスのボタンの押下をチェック
	/// </summary>
	/// <param name="mouseClicPos">マウスのボタン</param>
	/// <returns>押されてるか</returns>
	bool PressMouseButton(Click mouseClicPos);

	/// <summary>
	/// マウスのボタンの押下した瞬間をチェック
	/// </summary>
	/// <param name="mouseClickPos">マウスのボタン</param>
	/// <returns>押した瞬間</returns>
	bool TriggerMouseButton(Click mouseClickPos);

	/// <summary>
	/// マウスのボタンを離した瞬間をチェック
	/// </summary>
	/// <param name="mouseClickPos">マウスのボタン</param>
	/// <returns>離した瞬間</returns>
	bool ReleaseTriggerMouseButton(Click mouseClickPos);

	/// <summary>
	/// マウスの移動量のゲッター
	/// </summary>
	/// <returns>マウスの移動量</returns>
	Vector2Int GetMouseMoveAmount()const;

	/// <summary>
	/// マウスホイールの回転量のゲッター
	/// </summary>
	/// <returns>マウスホイールの回転量</returns>
	int32_t GetWheelRotate()const;

	/// <summary>
	/// ワールド座標系のマウスの位置のゲッター
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <returns>ワールド座標系のマウスの位置</returns>
	Vector3 GetWorldMousePosition(Camera* camera)const;

	/// <summary>
	/// スクリーン座標系のマウスの位置のゲッター
	/// </summary>
	/// <returns>スクリーン座標系のマウスの位置</returns>
	Vector2Int GetMousePosition()const;

	/// <summary>
	/// Xboxが接続できたかどうか
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <param name="joyState">Xボックスコントローラーのステータス</param>
	/// <returns>接続できたかどうか</returns>
	bool IsXboxPadConnected(DWORD xBoxPadNumber);

	/// <summary>
	/// XboxPadのボタンの押下をチェック
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <param name="xboxButton">Xboxのボタン</param>
	/// <returns>押されてるか</returns>
	bool PressXboxPad(DWORD xBoxPadNumber, XboxInput xboxButton);

	/// <summary>
	/// XboxPadのボタンの押下した瞬間をチェック
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <param name="xboxButton">Xboxのボタン</param>
	/// <returns>押した瞬間</returns>
	bool TriggerXboxPad(DWORD xBoxPadNumber, XboxInput xboxButton);

	/// <summary>
	/// XboxPadのボタンの離した瞬間をチェック
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <param name="xboxButton">Xboxのボタン</param>
	/// <returns>離した瞬間</returns>
	bool ReleaseTriggerXboxPad(DWORD xBoxPadNumber, XboxInput xboxButton);

	/// <summary>
	/// Xboxの左スティックのゲッター
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <returns>左スティック</returns>
	const Vector2 GetXboxPadLeftStick(DWORD xBoxPadNumber);

	/// <summary>
	/// Xboxの右スティックのゲッター
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <returns>右スティック</returns>
	const Vector2 GetXboxPadRighttStick(DWORD xBoxPadNumber);

	/// <summary>
	/// Xboxのデッドゾーン
	/// </summary>
	/// <param name="xBoxPadNumber">何番目(0~4)</param>
	/// <param name="deadZone">デッドゾーン</param>
	void SetDeadZone(DWORD xBoxPadNumber, float deadZone);
public://PrassKey
	class ConstructorKey {
		private:
		ConstructorKey() = default;
		friend class Core;
	};
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PrassKeyを受け取る</param>
	explicit Input(ConstructorKey);
private://メンバ関数
	//コピーコンストラクタを禁止
	Input(const Input&) = delete;
	//代入演算子を禁止
	const Input& operator=(const Input&) = delete;

	/// <summary>
	/// キーボード入力の初期化
	/// </summary>
	void KeyboardInitialize();

	/// <summary>
	/// キーボード入力の更新
	/// </summary>
	void KeyboardUpdate();

	/// <summary>
	/// マウス入力の初期化
	/// </summary>
	void MouseInitialize();

	/// <summary>
	/// マウス入力の更新
	/// </summary>
	void MouseUpdate();

	/// <summary>
	/// XboxPadの更新
	/// </summary>
	void XboxPadUpdate();
private://メンバ変数
	//ウィンドウズAPI
	WinApi* winApi_ = nullptr;
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	//全キーの入力状態を取得する
	BYTE keys_[256] = {};
	BYTE preKeys_[256] = {};
	//マウス
	ComPtr<IDirectInputDevice8>mouse_ = nullptr;
	DIMOUSESTATE mouseState_ = {};
	DIMOUSESTATE preMouseState_ = {};

	//XboxPad
	XboxPadData xboxPadDatas_[4] = {};
};

