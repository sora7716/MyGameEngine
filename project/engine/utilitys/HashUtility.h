#pragma once
#include <cstdint>

/// <summary>
/// ハッシュ関係
/// </summary>
class HashUtility{
public://メンバ関数
	/// <summary>
	/// ハッシュの作成
	/// </summary>
	/// <param name="seed"></param>
	/// <param name="value"></param>
	static void CreateHash(size_t& seed, int32_t value);
};

