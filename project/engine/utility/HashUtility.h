#pragma once
#include <cstdint>

/// <summary>
/// ハッシュ関係
/// </summary>
namespace HashUtility {
	/// <summary>
	/// ハッシュの作成
	/// </summary>
	/// <param name="seed"></param>
	/// <param name="value"></param>
	void CreateHash(size_t& seed, int32_t value);
};

