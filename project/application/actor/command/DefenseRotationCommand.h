#pragma once
#include "ICommand.h"

//前方宣言
class DefenseRotationComponent;

/// <summary>
/// ディフェンス用のオブジェクトの回転コマンド
/// </summary>
class DefenseRotationCommand :public ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DefenseRotationCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DefenseRotationCommand()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="rotateDir">回転方向</param>
	void Setup(float rotateDir);

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;

	/// <summary>
	/// ディフェンス用のオブジェクトの回転させるの設定
	/// </summary>
	/// <param name="defenseRotationComponent">ディフェンス用のオブジェクトの回転させる</param>
	void SetDefenseRotationComponent(DefenseRotationComponent* defenseRotationComponent);
private://メンバ変数
	//ディフェンス用のオブジェクトの回転させる
	DefenseRotationComponent* defenseRotationComponent_ = nullptr;
	//回転方向
	float rotateDir_ = 0.0f;
};

