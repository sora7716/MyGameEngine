#pragma once
#include <cstdint>
#include <vector>
#include <string>

//マップチップの状態
enum class MapChipType :uint32_t {
	kBlank, // 空白
	kBlock,  // ブロック
};

//マップチップの状態
struct MapChipStatus {
	MapChipType type = MapChipType::kBlank;
	uint32_t count = 0;
};

//マップチップデータ
struct MapChipData {
	std::vector<std::vector<MapChipStatus>> data;
};