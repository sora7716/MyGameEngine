#pragma once
#include "ICommand.h"
#include "Vector3.h"

//前方宣言
class MovementComponent;

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
	/// セットアップ
	/// </summary>
	/// <param name="direction">移動方向</param>
	void Setup(const Vector3& direction);

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;

	/// <summary>
	/// 移動用コンポーネントの設定
	/// </summary>
	/// <param name="movementComponent">移動用のコンポーネント</param>
	void SetMovementComponent(MovementComponent* movementComponent);
private://メンバ変数
	//移動用のコンポーネント
	MovementComponent* movementComponent_ = nullptr;
	//移動方向
	Vector3 direction_ = {};
};

