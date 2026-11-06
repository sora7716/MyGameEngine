#pragma once
#include <cstdint>
#include <vector>
#include <string>

//マップチップの状態
enum class MapChipType {
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

//マップチップフィールド
class MapChipField {
public://構造体
	//範囲矩形
	struct Rect {
		float left;//左端
		float right;//右端
		float bottom;//下端
		float top;//上端
	};
public://メンバ関数
	/// <summary>
	/// マップチップの読み込み
	/// </summary>
	/// <param name="filePath">ファイル名</param>
	void LoadMapChipCsv(const std::string& filePath);

	/// <summary>
	/// マップチップのデータを取得
	/// </summary>
	/// <returns>マップチップデータ</returns>
	const MapChipData& GetMapChipData()const;
private://メンバ関数
	/// <summary>
	/// マップチップのリセット
	/// </summary>
	void ResetMapChipData();
public://定数
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const float kBlockDepth = 1.0f;
private://メンバ変数
	//ディレクトリパス
	std::string directoryPath_ = "engine/resources/csv/";
	//マップチップのデータ
	MapChipData mapChipData_;
};
