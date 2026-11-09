#pragma once
#include "ICommand.h"

/// <summary>
/// 移動用のコマンド
/// </summary>
class MovementCommand :public ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MovementCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MovementCommand()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;
private://メンバ変数
};

