#pragma once
#include "ICommand.h"

//前方宣言
class PlacementSwitchComponent;

/// <summary>
/// 設置モデルを切り替えるコマンド
/// </summary>
class PlacementSwitchCommand :public ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlacementSwitchCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlacementSwitchCommand()override;

	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="dir">方向</param>
	void Setup(int32_t dir);

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;

	/// <summary>
	/// 設置モデルを切り替えるの設定
	/// </summary>
	/// <param name="placementSwitchComponent">設置モデルを切り替える</param>
	void SetPlacementSwitchComponent(PlacementSwitchComponent* placementSwitchComponent);
private://メンバ変数
	//設置モデルを切り替える
	PlacementSwitchComponent* placementSwitchComponent_ = nullptr;
	//切り替える方向
	int32_t dir_ = 0;
};
