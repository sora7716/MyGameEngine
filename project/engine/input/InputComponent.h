#pragma once
#include "Component.h"
#include "InputType.h"
#include "Vector2.h"
#include <variant>
#include <string>
#include <cstdint>
#include <vector>

//前方宣言
class Input;
class ICommand;

//入力したデバイスのタイプ
using InputDeviceType = std::variant<Click,BYTE,XboxInput>;

//ボタンの状態
enum class InputPhase :uint32_t {
	kPressed = 1 << 0,//押した瞬間「
	kHeld = 1 << 1,//押しっぱなし
	kReleased = 1 << 2,//離した瞬間
	kIdle = 1 << 3//離しっぱなし
};

/// <summary>
/// フェーズのビットフラグをOR演算
/// </summary>
/// <param name="phase1">フェーズ1</param>
/// <param name="phase2">フェーズ2</param>
/// <returns>OR演算したフェーズ</returns>
InputPhase operator| (InputPhase phase1,InputPhase phase2);

/// <summary>
/// フェーズのビットフラグをAND演算
/// </summary>
/// <param name="phase1">フェーズ1</param>
/// <param name="phase2">フェーズ2</param>
/// <returns>AND演算したフェーズ</returns>
InputPhase operator& (InputPhase phase1,InputPhase phase2);

//入力情報
struct InputState {
	InputDeviceType type;
	InputPhase phase;
	std::unique_ptr<ICommand> command;
	uint32_t controllerIndex;
};

/// <summary>
/// 入力処理のコンポーネント
/// </summary>
class InputComponent :public Component {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	explicit InputComponent(GameObject* gameObject);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~InputComponent()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	void Setup(Input* input);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 複製
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	/// <returns>コンポーネント</returns>
	std::unique_ptr<Component>Clone(GameObject* gameObject)const override;

	/// <summary>
	/// コンポーネントのタイプ名を取得
	/// </summary>
	/// <returns>コンポーネントのタイプ名</returns>
	std::string GetTypeName()const override;

	/// <summary>
	/// 入力状態の追加
	/// </summary>
	/// <param name="inputDeviceType">入力デバイス</param>
	/// <param name="phase">入力のフェーズ</param>
	/// <param name="command">コマンド</param>
	/// <param name="controllerIndex">コントローラの検索キー</param>
	void AddInputState(InputDeviceType inputDeviceType,InputPhase phase,std::unique_ptr<ICommand> command,uint32_t controllerIndex = 0);
private://メンバ関数
	/// <summary>
	/// 現在の入力フェーズを取得
	/// </summary>
	/// <param name="inputDeviceType">入力デバイス</param>
	/// <param name="controllerIndex">コントローラーの検索キー</param>
	InputPhase GetCurrentPhase(InputDeviceType inputDeviceType,uint32_t controllerIndex);
private://メンバ変数
	//入力
	Input* input_ = nullptr;

	//入力情報
	std::vector<InputState>inputStates_;
};

