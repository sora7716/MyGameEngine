#pragma once
#include <cstdint>
#include <vector>
#include "Vector3.h"

//マップチップで使用するデスク
namespace mapChipDesc {
	//マップチップの状態
	enum class MapChipType :uint32_t {
		kBlank, // 空白
		kBlock,  // ブロック
		kPlayer,//プレイヤー
	};

	//マップチップの状態
	struct MapChipStatus {
		MapChipType type = MapChipType::kBlank;
		uint32_t height = 0;
	};

	//マップチップデータ
	struct MapChipData {
		std::vector<std::vector<MapChipStatus>> data;
		Vector3Int playerIndex;//プレイヤーの位置のマップ番号
	};

	//範囲矩形
	struct Rect {
		float left;//左端
		float right;//右端
		float bottom;//下端
		float top;//上端
		float front;//前端
		float back;//後端
	};
};