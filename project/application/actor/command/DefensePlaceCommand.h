#pragma once
#include "ICommand.h"

//前方宣言
class DefensePlaceComponent;

/// <summary>
/// オブジェクトの設置を行うコマンド
/// </summary>
class DefensePlaceCommand :public ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DefensePlaceCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DefensePlaceCommand()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;

	/// <summary>
	/// ディフェンス用のオブジェクトを設置する機能の設定
	/// </summary>
	/// <param name="defenseRotationComponent">ディフェンス用のオブジェクトを設置する機能</param>
	void SetDefensePlaceComponent(DefensePlaceComponent* defensePlaceComponent);
private://メンバ変数
	//ディフェンス用のオブジェクトの回転させる
	DefensePlaceComponent* defensePlaceComponent_ = nullptr;
};


