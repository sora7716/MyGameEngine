#pragma once
#include "Vector2.h"
#include <memory>

/// <summary>
/// 軸入力用のコマンド
/// </summary>
class IAxisCommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IAxisCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IAxisCommand();

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="axis">入力値(-1~1)</param>
	virtual void Execute(const Vector2& axis) = 0;

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns></returns>
	virtual std::unique_ptr<IAxisCommand>Clone()const = 0;
};

