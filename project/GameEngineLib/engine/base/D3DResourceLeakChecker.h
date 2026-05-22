#pragma once

/// <summary>
/// メモリリークをチェック
/// </summary>
class D3DResourceLeakChecker{
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	D3DResourceLeakChecker() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~D3DResourceLeakChecker();
};

