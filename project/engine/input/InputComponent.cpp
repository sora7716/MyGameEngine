#include "InputComponent.h"
#include "Input.h"
#include "ICommand.h"
#include "Logger.h"
#include <cassert>
//フェーズのビットフラグをOR演算
InputPhase operator|(InputPhase phase1,InputPhase phase2) {
	uint32_t phaseNum1 = static_cast<uint32_t>(phase1);
	uint32_t phaseNum2 = static_cast<uint32_t>(phase2);

	uint32_t result = phaseNum1 | phaseNum2;
	return static_cast<InputPhase>(result);
}

//フェーズのビットフラグをAND演算
InputPhase operator&(InputPhase phase1,InputPhase phase2) {
	uint32_t phaseNum1 = static_cast<uint32_t>(phase1);
	uint32_t phaseNum2 = static_cast<uint32_t>(phase2);

	uint32_t result = phaseNum1 & phaseNum2;
	return static_cast<InputPhase>(result);
}

//コンストラクタ
InputComponent::InputComponent(GameObject* gameObject) :Component(gameObject) {
}

//デストラクタ
InputComponent::~InputComponent() {
}

//セットアップ
void InputComponent::Setup(Input* input) {
	assert(input);
	input_ = input;
}

//初期化
void InputComponent::Initialize() {

}

//更新
void InputComponent::Update() {
	//入力がNullなら
	if(!input_) {
		Logger::OutputLog("inputが設定されていません");
		return;
	}

	//ステートを更新
	for(InputState& inputState : inputStates_) {
		//現在のフェーズを取得
		InputPhase currentPhase = GetCurrentPhase(inputState.type,inputState.controllerIndex);

		//現在のフェーズが許可されているか
		bool isPhaseAllowed = static_cast<uint32_t>(currentPhase & inputState.phase) != 0u;
		if(isPhaseAllowed) {
			//コマンドの実行
			inputState.command->Execute();
		}
	}
}

//複製
std::unique_ptr<Component> InputComponent::Clone(GameObject* gameObject) const {
	std::unique_ptr<InputComponent>cloneInstance = std::make_unique<InputComponent>(gameObject);

	//初期化
	cloneInstance->Initialize();
	cloneInstance->input_ = this->input_;
	for(uint32_t i = 0; i < this->inputStates_.size(); i++) {
		const InputState& inputState = this->inputStates_[i];
		std::unique_ptr<ICommand> cloneCommand = inputState.command->Clone();
		cloneInstance->inputStates_.push_back({inputState.type,inputState.phase,std::move(cloneCommand),inputState.controllerIndex});
	}

	//Playerが持つ設定だけ複製
	cloneInstance->SetEnabled(this->IsEnabled());
	return cloneInstance;
}

//コンポーネントのタイプ名を取得
std::string InputComponent::GetTypeName() const {
	return "InputComponent";
}

//入力状態の追加
void InputComponent::AddInputState(InputDeviceType inputDeviceType,InputPhase phase,std::unique_ptr<ICommand> command,uint32_t controllerIndex) {
	//コマンドがNullなら　
	if(!command) {
		return;
	}

	//コントローラの検索キーが範囲外なら
	if(controllerIndex >= 4) {
		return;
	}

	inputStates_.push_back({inputDeviceType,phase,std::move(command),controllerIndex});
}

//現在の入力フェーズを取得
InputPhase InputComponent::GetCurrentPhase(InputDeviceType inputDeviceType,uint32_t controllerIndex) {

	//各ポインタを取得
	BYTE* bytePtr = std::get_if<BYTE>(&inputDeviceType);
	Click* ClickPtr = std::get_if<Click>(&inputDeviceType);
	XboxInput* xboxInputPtr = std::get_if<XboxInput>(&inputDeviceType);
	//バイトのポインタがある場合
	if(bytePtr) {
		//押した瞬間
		if(input_->TriggerKey(*bytePtr)) {
			return InputPhase::kPressed;
		}

		//押している間
		if(input_->PressKey(*bytePtr)) {
			return InputPhase::kHeld;
		}

		//離した瞬間
		if(input_->ReleaseTriggerKey(*bytePtr)) {
			return InputPhase::kReleased;
		}
	}

	//クリックのポインタがある場合
	if(ClickPtr) {
		//押した瞬間
		if(input_->TriggerMouseButton(*ClickPtr)) {
			return InputPhase::kPressed;
		}

		//押している間
		if(input_->PressMouseButton(*ClickPtr)) {
			return InputPhase::kHeld;
		}

		//離した瞬間
		if(input_->ReleaseTriggerMouseButton(*ClickPtr)) {
			return InputPhase::kReleased;
		}
	}

	//XBoxのポインタがある場合
	if(xboxInputPtr) {
		//押した瞬間
		if(input_->TriggerXboxPad(controllerIndex,*xboxInputPtr)) {
			return InputPhase::kPressed;
		}

		//押している間
		if(input_->PressXboxPad(controllerIndex,*xboxInputPtr)) {
			return InputPhase::kHeld;
		}

		//離した瞬間
		if(input_->ReleaseTriggerXboxPad(controllerIndex,*xboxInputPtr)) {
			return InputPhase::kReleased;
		}
	}

	return InputPhase::kIdle;
}
