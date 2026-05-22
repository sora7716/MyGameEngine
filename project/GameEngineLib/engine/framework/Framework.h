#pragma once
#include "Core.h"

/// <summary>
/// ゲーム全体
/// </summary>
class Framework {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Framework() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Framework() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// ゲームループ
	/// </summary>
	void Run();

	/// <summary>
	/// 終了リクエスト
	/// </summary>
	/// <returns>終了したかどうか</returns>
	virtual bool isEndRequest();
protected://メンバ変数
	//エンジンの核
	std::unique_ptr<Core>core_ = nullptr;
};

