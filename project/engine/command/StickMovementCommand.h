#pragma once
#include "IAxisCommand.h"

//前方宣言
class MovementComponent;

/// <summary>
/// スティックでの移動コマンド
/// </summary>
class StickMovementCommand:public IAxisCommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StickMovementCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StickMovementCommand()override;

	/// <summary>
	/// 実行
	/// </summary>
	/// <param name="axis">入力値(-1~1)</param>
	void Execute(const Vector3& axis);

	/// <summary>
	/// 複製
	/// </summary>
	/// <returns></returns>
	std::unique_ptr<IAxisCommand>Clone()const; 

	/// <summary>
	/// 移動用コンポーネントの設定
	/// </summary>
	/// <param name="movementComponent">移動用のコンポーネント</param>
	void SetMovementComponent(MovementComponent* movementComponent);
private://メンバ変数
	//移動用のコンポーネント
	MovementComponent* movementComponent_ = nullptr;
};

