#pragma once
#include "ICommand.h"
#include "Vector3.h"

//前方宣言
class JumpComponent;

/// <summary>
/// ジャンプコマンド
/// </summary>
class JumpCommand :public ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	JumpCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~JumpCommand()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;

	/// <summary>
	/// ジャンプ用コンポーネントの設定
	/// </summary>
	/// <param name="jumpComponent">ジャンプ用のコンポーネント</param>
	void SetJumpComponent(JumpComponent* jumpComponent);
private://メンバ変数
	//ジャンプ用のコンポーネント
	JumpComponent* jumpComponent_ = nullptr;
};


