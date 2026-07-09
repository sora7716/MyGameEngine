#include "HashUtility.h"
#include <functional>

//ハッシュの作成
void HashUtility::CreateHash(size_t& seed, int32_t value) {
	seed ^= std::hash<int32_t>{}(value)+0x9e3779b9 + (seed << 6) + (seed >> 2);
}
