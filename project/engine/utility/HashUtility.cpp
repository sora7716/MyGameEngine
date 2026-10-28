#include "HashUtility.h"
#include <functional>

//ハッシュの作成
void hashUtility::CreateHash(size_t& seed, size_t value) {
	seed ^= std::hash<size_t>{}(value)+0x9e3779b9 + (seed << 6) + (seed >> 2);
}
