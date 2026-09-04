#pragma once
#include "Vector3.h"
#include <array>
#include <cstdint>

//マップデータ
struct MapData{
	std::array<std::array<std::array<uint32_t, 6>, 6>, 3> map;
};

//タイルタイプ
enum class TileType{
	kEmpty,
	kBlock
};

/// <summary>
/// タイルマップ
/// </summary>
class TileMap{
public://メンバ関数
	/// <summary>
	/// タイルタイプを取得
	/// </summary>
	/// <returns>マップデータからタイプタイプを取得</returns>	
	TileType GetTileTypeForMapData();
private://メンバ変数
	//マップデータ
	MapData map_;
	//ブロックの幅
	Vector3 tileSize_ = { 1.0f,1.0f,1.0f };
};

