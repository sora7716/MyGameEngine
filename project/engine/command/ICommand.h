#pragma once
#include "GameObject.h"

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
	/// セットアップ
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト/param>
	virtual void Setup(GameObject* gameObject);

	/// <summary>
	///　実行
	/// </summary>
	virtual void Execute() = 0;

	/// <summary>
	/// 複製
	/// </summary>
	virtual std::unique_ptr<ICommand> Clone()const = 0;
protected://メンバ変数
	//ゲームオブジェクト
	GameObject* gameObject_ = nullptr;
};

