#pragma once
#include "ICommand.h"
#include <memory>

//前方宣言
class PlacementPreviewComponent;

/// <summary>
/// 設置予定地の表示/非表示コマンド
/// </summary>
class PlacementPreviewCommand :public ICommand {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlacementPreviewCommand();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlacementPreviewCommand()override;

	/// <summary>
	/// 実行
	/// </summary>
	void Execute()override;

	/// <summary>
	/// 複製
	/// </summary>
	std::unique_ptr<ICommand> Clone()const override;

	/// <summary>
	/// 置く場所の予想位置を表示する用の設定
	/// </summary>
	/// <param name="placementPreviewComponent">置く場所の予想位置を表示する用</param>
	void SetPlacementPreviewComponent(PlacementPreviewComponent* placementPreviewComponent);
private://メンバ変数
	//置く場所の予想位置を表示する用
	PlacementPreviewComponent* placementPreviewComponent_ = nullptr;
	//表示非表示のフラグ
	bool isVisible_ = false;
};

