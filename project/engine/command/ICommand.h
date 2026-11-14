#pragma once
#include <memory>

/// <summary>
/// コマンドのインターフェース
/// </summary>
class ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ICommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ICommand();

	/// <summary>
	///　実行
	/// </summary>
	virtual void Execute() = 0;

	/// <summary>
	/// 複製
	/// </summary>
	virtual std::unique_ptr<ICommand> Clone()const = 0;
};

